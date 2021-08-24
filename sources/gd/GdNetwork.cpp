// SPDX-License-Identifier: AGPL-3.0-or-later
#include "GdNetwork.h"
#include "utility/Volume.h"
#include <simde/x86/sse.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cassert>

GdNetwork::GdNetwork(ChannelMode channelMode)
{
    switch (channelMode) {
    case Mono:
        channels_.resize(1);
        break;
    case Stereo:
        channels_.resize(2);
        break;
    default:
        assert(false);
    }

    smoothFbGainLinear_.setSmoothTime(GdParamSmoothTime);

    for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex)
        smoothTapLatency_[tapIndex].setSmoothTime(GdParamSmoothTime);
}

GdNetwork::~GdNetwork()
{
}

void GdNetwork::clear()
{
    smoothFbGainLinear_.clear(db2linear(fbTapGainDB_));

    for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex)
        smoothTapLatency_[tapIndex].clear(channels_[0].taps_[tapIndex].fx_.getLatency());

    for (ChannelDsp &chan : channels_)
        chan.clear();

    for (TapControl &tapControl : tapControls_)
        tapControl.clear();
}

void GdNetwork::setSampleRate(float sampleRate)
{
    smoothFbGainLinear_.setSampleRate(sampleRate);

    for (ChannelDsp &chan : channels_)
        chan.setSampleRate(sampleRate);

    for (TapControl &tapControl : tapControls_)
        tapControl.setSampleRate(sampleRate);
}

void GdNetwork::setBufferSize(unsigned bufferSize)
{
    for (std::vector<float> &temp : temp_)
        temp.resize(bufferSize);

    for (ChannelDsp &chan : channels_)
        chan.setBufferSize(bufferSize);
}

void GdNetwork::setParameter(unsigned parameter, float value)
{
    if (parameter < GDP_TAP_A_ENABLE) {
        switch (parameter) {
        case GDP_SYNC:
            sync_ = (bool)value;
            break;
        case GDP_GRID:
            div_ = GdFindNearestDivisor(value);
            break;
        case GDP_FEEDBACK_ENABLE:
            fbEnable_ = (bool)value;
            break;
        case GDP_FEEDBACK_TAP:
            fbTapIndex_ = (unsigned)value;
            break;
        case GDP_FEEDBACK_GAIN:
            fbTapGainDB_ = value;
            break;
        }
    }
    else {
        unsigned numTapParameters = GDP_TAP_B_ENABLE - GDP_TAP_A_ENABLE;
        unsigned tapIndex = (parameter - GDP_TAP_A_ENABLE) / numTapParameters;

        TapControl &tapControl = tapControls_[tapIndex];

        switch (parameter - tapIndex * numTapParameters) {
        case GDP_TAP_A_ENABLE:
            if (!value)
                tapControl.enable_ = false;
            else if (!tapControl.enable_) {
                tapControl.enable_ = true;
                for (ChannelDsp &chan : channels_)
                    chan.taps_[tapIndex].clear();
                tapControl.clear();
            }
            break;
        case GDP_TAP_A_DELAY:
            tapControl.delay_ = value;
            break;
        case GDP_TAP_A_LEVEL:
            tapControl.levelDB_ = value;
            break;
        case GDP_TAP_A_MUTE:
            tapControl.mute_ = (bool)value;
            break;
        case GDP_TAP_A_FILTER_ENABLE:
            tapControl.filterEnable_ = (bool)value;
            break;
        case GDP_TAP_A_FILTER:
            tapControl.filter_ = (int)value;
            break;
        case GDP_TAP_A_LPF_CUTOFF:
            tapControl.lpfCutoff_ = value;
            break;
        case GDP_TAP_A_HPF_CUTOFF:
            tapControl.hpfCutoff_ = value;
            break;
        case GDP_TAP_A_RESONANCE:
            tapControl.resonanceDB_ = value;
            break;
        case GDP_TAP_A_TUNE_ENABLE:
            tapControl.shiftEnable_ = (bool)value;
            break;
        case GDP_TAP_A_TUNE:
            tapControl.shift_ = value;
            break;
        case GDP_TAP_A_PAN:
            tapControl.pan_ = value;
            break;
        case GDP_TAP_A_WIDTH:
            tapControl.width_ = value;
            break;
        case GDP_TAP_A_FLIP:
            tapControl.flip_ = (bool)value;
            break;
        }
    }
}

void GdNetwork::setTempo(float tempo)
{
    bpm_ = tempo;
}

void GdNetwork::process(const float *const inputs[], const float *dry, const float *wet, float *const outputs[], unsigned count)
{
    const ChannelDsp *channels = channels_.data();
    unsigned numInputs = (unsigned)channels_.size();

    unsigned fbTapIndex = fbTapIndex_;

    const float *leftInput = inputs[0];
    const float *rightInput = (numInputs == 2) ? inputs[1] : inputs[0];
    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    size_t iTemp = 0;
    auto allocateTemp = [this, &iTemp]() -> float * {
        assert(iTemp < kNumTempBuffers);
        return temp_[iTemp++].data();
    };

    float *delays = allocateTemp();
    float *feedbackGain = allocateTemp();
    float *level = allocateTemp();
    float *pan = allocateTemp();
    float *width = allocateTemp();
    float *feedbackTapOutputs[2] = { allocateTemp(), allocateTemp() };
    float *ordinaryTapOutputs[2] = { allocateTemp(), allocateTemp() };
    float *inputAndFeedbackSums[2] = { allocateTemp(), allocateTemp() };

    GdTapFx::Control fxControl;
    fxControl.lpfCutoff = allocateTemp();
    fxControl.hpfCutoff = allocateTemp();
    fxControl.resonance = allocateTemp();
    fxControl.shift = allocateTemp();

    float *latency = allocateTemp();

    const float *tapInputs[2] = { leftInput, rightInput };

    // fill outputs with dry signal
    for (unsigned i = 0; i < count; ++i) {
        float gain = dry[i];
        leftOutput[i] = gain * leftInput[i];
        rightOutput[i] = gain * rightInput[i];
    }

    // skip processing the feedback if disabled
    bool maySkipFeedback = !fbEnable_ || fbTapGainDB_ <= GdMinFeedbackGainDB;
    if (maySkipFeedback && smoothFbGainLinear_.current() <= GdMinFeedbackGainLinear) {
        fbTapIndex = ~0u;
    }

    // if there is a feedback line, process it first
    if (fbTapIndex == ~0u) {
        for (unsigned chanIndex = 0; chanIndex < numInputs; ++chanIndex)
            std::fill_n(feedbackTapOutputs[chanIndex], count, 0.0f);
    }
    else {
        TapControl &tapControl = tapControls_[fbTapIndex];

        if (tapControl.enable_) {
            // compute the line delays
            float tapDelay = tapControl.delay_;
            if (sync_)
                tapDelay = GdAlignDelayToGrid(tapDelay, div_, bpm_);
            std::fill_n(delays, count, tapDelay);
            tapControl.smoothDelay_.process(delays, delays, count, true);

            // compute tap latency
            std::fill_n(latency, count, channels_[0].taps_[fbTapIndex].fx_.getLatency());
            smoothTapLatency_[fbTapIndex].process(latency, latency, count);

            // compensate delays according to latency
            for (unsigned i = 0; i < count; ++i)
                delays[i] = std::max(0.0f, delays[i] - latency[i]);

            // calculate level
            std::fill_n(level, count, tapControl.mute_ ? 0.0f : db2linear(tapControl.levelDB_));
            tapControl.smoothLevelLinear_.process(level, level, count, true);

            // calculate pan
            std::fill_n(pan, count, tapControl.flip_ ? -tapControl.pan_ : tapControl.pan_);
            tapControl.smoothPan_.process(pan, pan, count, true);

            // calculate width (stereo only)
            if (numInputs == 2) {
                std::fill_n(width, count, tapControl.width_);
                tapControl.smoothWidth_.process(width, width, count, true);
            }

            // compute FX parameters
            fxControl.filter = tapControl.filterEnable_ ? tapControl.filter_ : GdFilterOff;
            std::fill_n(fxControl.lpfCutoff, count, tapControl.lpfCutoff_);
            tapControl.smoothLpfCutoff_.process(fxControl.lpfCutoff, fxControl.lpfCutoff, count, true);
            std::fill_n(fxControl.hpfCutoff, count, tapControl.hpfCutoff_);
            tapControl.smoothHpfCutoff_.process(fxControl.hpfCutoff, fxControl.hpfCutoff, count, true);
            std::fill_n(fxControl.resonance, count, db2linear(tapControl.resonanceDB_));
            tapControl.smoothResonanceLinear_.process(fxControl.resonance, fxControl.resonance, count, true);
            std::fill_n(fxControl.shift, count, tapControl.shiftEnable_ ? std::exp2((1.0f / 1200) * tapControl.shift_) : 1.0f);
            tapControl.smoothShiftLinear_.process(fxControl.shift, fxControl.shift, count, true);

            // compute the feedback gain
            std::fill_n(feedbackGain, count, fbEnable_ ? db2linear(fbTapGainDB_) : 0.0f);
            smoothFbGainLinear_.process(feedbackGain, feedbackGain, count, true);

            for (unsigned chanIndex = 0; chanIndex < numInputs; ++chanIndex) {
                ChannelDsp &chan = channels_[chanIndex];
                TapDsp &tap = chan.taps_[fbTapIndex];
                float feedback = chan.feedback_;

                // compute the line and its effects
                const float *input = inputs[chanIndex];
                float *inputAndFeedbackSum = inputAndFeedbackSums[chanIndex];
                float *feedbackTapOutput = feedbackTapOutputs[chanIndex];

                unsigned i = 0;
                GdTapFx &fx = tap.fx_;

                while (i + GdTapFx::kControlUpdateInterval < count) {
                    fx.performKRateUpdates(fxControl, i);
                    for (unsigned j = i + GdTapFx::kControlUpdateInterval; i < j; ++i) {
                        float in = input[i] + feedback * feedbackGain[i];
                        inputAndFeedbackSum[i] = in;
                        float out = tap.line_.processOne(in, delays[i]);
                        out = fx.processOne(out, fxControl, i);
                        feedbackTapOutput[i] = out;
                        feedback = out;
                    }
                }
                if (i < count) {
                    fx.performKRateUpdates(fxControl, i);
                    for (; i < count; ++i) {
                        float in = input[i] + feedback * feedbackGain[i];
                        inputAndFeedbackSum[i] = in;
                        float out = tap.line_.processOne(in, delays[i]);
                        out = fx.processOne(out, fxControl, i);
                        feedbackTapOutput[i] = out;
                        feedback = out;
                    }
                }

                // use this as input to the rest of taps
                tapInputs[chanIndex] = inputAndFeedbackSum;

                chan.feedback_ = feedback;
            }
        }
    }

    for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex) {
        TapControl &tapControl = tapControls_[tapIndex];

        if (!tapControl.enable_)
            continue;

        if (tapIndex == fbTapIndex) {
            // add to stereo mix
            if (numInputs == 2)
                mixStereoToStereo(tapIndex, feedbackTapOutputs, level, pan, width, wet, outputs, count);
            else
                mixMonoToStereo(tapIndex, feedbackTapOutputs[0], level, pan, wet, outputs, count);
        }
        else {
            // compute the line delays
            float tapDelay = tapControl.delay_;
            if (sync_)
                tapDelay = GdAlignDelayToGrid(tapDelay, div_, bpm_);
            std::fill_n(delays, count, tapDelay);
            tapControl.smoothDelay_.process(delays, delays, count, true);

            // compute tap latency
            std::fill_n(latency, count, channels_[0].taps_[tapIndex].fx_.getLatency());
            smoothTapLatency_[tapIndex].process(latency, latency, count);

            // compensate delays according to latency
            for (unsigned i = 0; i < count; ++i)
                delays[i] = std::max(0.0f, delays[i] - latency[i]);

            // calculate level
            std::fill_n(level, count, tapControl.mute_ ? 0.0f : db2linear(tapControl.levelDB_));
            tapControl.smoothLevelLinear_.process(level, level, count, true);

            // calculate pan
            std::fill_n(pan, count, tapControl.flip_ ? -tapControl.pan_ : tapControl.pan_);
            tapControl.smoothPan_.process(pan, pan, count, true);

            // calculate width (stereo only)
            if (numInputs == 2) {
                std::fill_n(width, count, tapControl.width_);
                tapControl.smoothWidth_.process(width, width, count, true);
            }

            // compute FX parameters
            fxControl.filter = tapControl.filterEnable_ ? tapControl.filter_ : GdFilterOff;
            std::fill_n(fxControl.lpfCutoff, count, tapControl.lpfCutoff_);
            tapControl.smoothLpfCutoff_.process(fxControl.lpfCutoff, fxControl.lpfCutoff, count, true);
            std::fill_n(fxControl.hpfCutoff, count, tapControl.hpfCutoff_);
            tapControl.smoothHpfCutoff_.process(fxControl.hpfCutoff, fxControl.hpfCutoff, count, true);
            std::fill_n(fxControl.resonance, count, db2linear(tapControl.resonanceDB_));
            tapControl.smoothResonanceLinear_.process(fxControl.resonance, fxControl.resonance, count, true);
            std::fill_n(fxControl.shift, count, tapControl.shiftEnable_ ? std::exp2((1.0f / 1200) * tapControl.shift_) : 1.0f);
            tapControl.smoothShiftLinear_.process(fxControl.shift, fxControl.shift, count, true);

            for (unsigned chanIndex = 0; chanIndex < numInputs; ++chanIndex) {
                ChannelDsp &chan = channels_[chanIndex];
                TapDsp &tap = chan.taps_[tapIndex];

                // compute the line and its effects
                const float *tapInput = tapInputs[chanIndex];
                float *ordinaryTapOutput = ordinaryTapOutputs[chanIndex];

                tap.line_.process(tapInput, delays, ordinaryTapOutput, count);

                unsigned i = 0;
                GdTapFx &fx = tap.fx_;
                for (; i + GdTapFx::kControlUpdateInterval < count; i += GdTapFx::kControlUpdateInterval) {
                    fx.performKRateUpdates(fxControl, i);
                    fx.process(ordinaryTapOutput + i, ordinaryTapOutput + i, fxControl, GdTapFx::kControlUpdateInterval);
                }
                if (i < count) {
                    fx.performKRateUpdates(fxControl, i);
                    fx.process(ordinaryTapOutput + i, ordinaryTapOutput + i, fxControl, count - i);
                }
            }

            // add to stereo mix
            if (numInputs == 2)
                mixStereoToStereo(tapIndex, ordinaryTapOutputs, level, pan, width, wet, outputs, count);
            else
                mixMonoToStereo(tapIndex, ordinaryTapOutputs[0], level, pan, wet, outputs, count);
        }
    }
}

//==============================================================================
static inline simde__m128 calcStereoPanGains(float value)
{
    value = 0.5f * (value + 1.0f);
    simde__m128 x = simde_mm_setr_ps(1.0f - value, value, 0.0f, 0.0f);
    simde__m128 y = simde_mm_sqrt_ps(x);
    return y;
}

//==============================================================================
void GdNetwork::mixMonoToStereo(unsigned tapIndex, const float *input, const float *level, const float *pans, const float *wet, float *const outputs[], unsigned count)
{
    // TODO level

    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    for (unsigned i = 0; i < count; ++i) {
        simde__m128 panGain = calcStereoPanGains(pans[i]);

        float in = input[i];
        float gain = wet[i] * level[i];
        float leftSample = in * gain * ((float *)&panGain)[0];
        float rightSample = in * gain * ((float *)&panGain)[1];

        leftOutput[i] += leftSample;
        rightOutput[i] += rightSample;
    }
}

void GdNetwork::mixStereoToStereo(unsigned tapIndex, const float *const inputs[], const float *level, const float *pans, const float *widths, const float *wet, float *const outputs[], unsigned count)
{
    // TODO level

    const float *leftInput = inputs[0];
    const float *rightInput = inputs[1];
    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    for (unsigned i = 0; i < count; ++i) {
        simde__m128 panGain = calcStereoPanGains(pans[i]);

        float gain = wet[i] * level[i];
        float leftSample = leftInput[i] * gain * ((float *)&panGain)[0];
        float rightSample = rightInput[i] * gain * ((float *)&panGain)[1];

        float mid = 0.5f * (leftSample + rightSample);
        float side = 0.5f * (rightSample - leftSample);
        float width = widths[i];
        float att = std::max(1.0f + width, 2.0f);
        leftSample = (mid - width * side) / att;
        rightSample = (mid + width * side) / att;

        leftOutput[i] += leftSample;
        rightOutput[i] += rightSample;
    }
}

//==============================================================================
GdNetwork::TapDsp::TapDsp()
{
    line_.setMaxDelay(GdMaxDelay);
}

void GdNetwork::TapDsp::clear()
{
    line_.clear();
    fx_.clear();
}

void GdNetwork::TapDsp::setSampleRate(float sampleRate)
{
    line_.setSampleRate(sampleRate);
    fx_.setSampleRate(sampleRate);
}

void GdNetwork::TapDsp::setBufferSize(unsigned bufferSize)
{
    fx_.setBufferSize(bufferSize);
}

//==============================================================================
void GdNetwork::ChannelDsp::clear()
{
    feedback_ = 0;

    for (TapDsp &tap : taps_)
        tap.clear();
}

void GdNetwork::ChannelDsp::setSampleRate(float sampleRate)
{
    for (TapDsp &tap : taps_)
        tap.setSampleRate(sampleRate);
}

void GdNetwork::ChannelDsp::setBufferSize(unsigned bufferSize)
{
    for (TapDsp &tap : taps_)
        tap.setBufferSize(bufferSize);
}

//==============================================================================
GdNetwork::TapControl::TapControl()
{
    for (LinearSmoother *smoother : getSmoothers())
        smoother->setSmoothTime(GdParamSmoothTime);
}

void GdNetwork::TapControl::clear()
{
    std::array<LinearSmoother *, kNumSmoothers> smoothers = getSmoothers();
    std::array<float, kNumSmoothers> targets = getSmootherTargets();
    for (unsigned i = 0; i < kNumSmoothers; ++i)
        smoothers[i]->clear(targets[i]);
}

void GdNetwork::TapControl::setSampleRate(float sampleRate)
{
    for (LinearSmoother *smoother : getSmoothers())
        smoother->setSampleRate(sampleRate);
}

auto GdNetwork::TapControl::getSmoothers() -> std::array<LinearSmoother *, kNumSmoothers>
{
    return {{
        &smoothDelay_,
        &smoothLevelLinear_,
        &smoothLpfCutoff_,
        &smoothHpfCutoff_,
        &smoothResonanceLinear_,
        &smoothShiftLinear_,
        &smoothPan_,
        &smoothWidth_,
    }};
}

auto GdNetwork::TapControl::getSmootherTargets() -> std::array<float, kNumSmoothers>
{
    return {{
        delay_,
        db2linear(levelDB_),
        lpfCutoff_,
        hpfCutoff_,
        db2linear(resonanceDB_),
        std::exp2(shift_ * (1.0f / 1200)),
        pan_,
        width_,
    }};
}
