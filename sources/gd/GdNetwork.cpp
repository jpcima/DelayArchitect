/* Delay Architect
 * Copyright (C) 2021  Jean Pierre Cimalando
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

    smoothFbGainLinear_.setTimeConstant(GdParamSmoothTime);

    for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex)
        smoothTapLatency_[tapIndex].setTimeConstant(GdParamSmoothTime);
}

GdNetwork::~GdNetwork()
{
}

void GdNetwork::clear()
{
    smoothFbGainLinear_.clearToTarget();

    for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex) {
        smoothTapLatency_[tapIndex].setTarget(channels_[0].taps_[tapIndex].fx_.getLatency());
        smoothTapLatency_[tapIndex].clearToTarget();
    }

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
    for (TempBuffer &temp : temp_)
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
        all_tap_delays:
            for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex) {
                TapControl &tapControl = tapControls_[tapIndex];
                tapControl.smoothDelay_.setTarget(!sync_ ? tapControl.delay_ :
                    GdAlignDelayToGrid(tapControl.delay_, div_, swing_, bpm_));
            }
            break;
        case GDP_GRID:
            div_ = GdFindNearestDivisor(value);
            goto all_tap_delays;
        case GDP_SWING:
            swing_ = value / 100.0f;
            goto all_tap_delays;
        case GDP_FEEDBACK_ENABLE:
            fbEnable_ = (bool)value;
            goto feedback_gain;
        case GDP_FEEDBACK_TAP:
            fbTapIndex_ = (unsigned)value;
            break;
        case GDP_FEEDBACK_GAIN:
            fbTapGainDB_ = value;
        feedback_gain:
            smoothFbGainLinear_.setTarget(!fbEnable_ ? 0.0f :
                (fbTapGainDB_ <= GdMinFeedbackGainDB) ? 0.0f :
                db2linear(fbTapGainDB_));
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
            {
                tapControl.delay_ = value;
                tapControl.smoothDelay_.setTarget(!sync_ ? tapControl.delay_ :
                    GdAlignDelayToGrid(tapControl.delay_, div_, swing_, bpm_));
            }
            break;
        case GDP_TAP_A_LEVEL:
            tapControl.levelDB_ = value;
        tap_level:
            tapControl.smoothLevelLinear_.setTarget(tapControl.mute_ ? 0.0f : db2linear(tapControl.levelDB_));
            break;
        case GDP_TAP_A_MUTE:
            tapControl.mute_ = (bool)value;
            goto tap_level;
        case GDP_TAP_A_FILTER_ENABLE:
            tapControl.filterEnable_ = (bool)value;
            break;
        case GDP_TAP_A_FILTER:
            tapControl.filter_ = (int)value;
            break;
        case GDP_TAP_A_LPF_CUTOFF:
            tapControl.lpfCutoff_ = value;
            tapControl.smoothLpfCutoff_.setTarget(tapControl.lpfCutoff_);
            break;
        case GDP_TAP_A_HPF_CUTOFF:
            tapControl.hpfCutoff_ = value;
            tapControl.smoothHpfCutoff_.setTarget(tapControl.hpfCutoff_);
            break;
        case GDP_TAP_A_RESONANCE:
            tapControl.resonanceDB_ = value;
            tapControl.smoothResonanceLinear_.setTarget(db2linear(tapControl.resonanceDB_));
            break;
        case GDP_TAP_A_TUNE_ENABLE:
            tapControl.shiftEnable_ = (bool)value;
            goto tap_tune;
        case GDP_TAP_A_TUNE:
            tapControl.shift_ = value;
        tap_tune:
            tapControl.smoothShiftLinear_.setTarget(tapControl.shiftEnable_ ? std::exp2((1.0f / 1200) * tapControl.shift_) : 1.0f);
            break;
        case GDP_TAP_A_PAN:
            tapControl.pan_ = value / 100.0f;
        tap_pan:
            {
                float flipped = tapControl.flip_ ? -tapControl.pan_ : tapControl.pan_;
                tapControl.smoothPanNormalized_.setTarget((flipped + 1) / 2);
            }
            break;
        case GDP_TAP_A_WIDTH:
            tapControl.width_ = value / 100.0f;
            tapControl.smoothWidth_.setTarget(tapControl.width_);
            break;
        case GDP_TAP_A_FLIP:
            tapControl.flip_ = (bool)value;
            goto tap_pan;
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
    if (smoothFbGainLinear_.getTarget() == 0.0f && smoothFbGainLinear_.getCurrentValue() == 0.0f) {
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
            tapControl.smoothDelay_.nextBlock(delays, count);

            // compute tap latency
            smoothTapLatency_[fbTapIndex].setTarget(channels_[0].taps_[fbTapIndex].fx_.getLatency());
            smoothTapLatency_[fbTapIndex].nextBlock(latency, count);

            // compensate delays according to latency
            for (unsigned i = 0; i < count; ++i)
                delays[i] = std::max(0.0f, delays[i] - latency[i]);

            // calculate level
            tapControl.smoothLevelLinear_.nextBlock(level, count);

            // calculate pan
            tapControl.smoothPanNormalized_.nextBlock(pan, count);

            // calculate width (stereo only)
            if (numInputs == 2)
                tapControl.smoothWidth_.nextBlock(width, count);

            // compute FX parameters
            fxControl.filter = tapControl.filterEnable_ ? tapControl.filter_ : GdFilterOff;
            tapControl.smoothLpfCutoff_.nextBlock(fxControl.lpfCutoff, count);
            tapControl.smoothHpfCutoff_.nextBlock(fxControl.hpfCutoff, count);
            tapControl.smoothResonanceLinear_.nextBlock(fxControl.resonance, count);
            tapControl.smoothShiftLinear_.nextBlock(fxControl.shift, count);

            // compute the feedback gain
            smoothFbGainLinear_.nextBlock(feedbackGain, count);

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
                        //out = cubicNL(out); // saturate feedback
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
                        //out = cubicNL(out); // saturate feedback
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
            tapControl.smoothDelay_.nextBlock(delays, count);

            // compute tap latency
            smoothTapLatency_[tapIndex].setTarget(channels_[0].taps_[tapIndex].fx_.getLatency());
            smoothTapLatency_[tapIndex].nextBlock(latency, count);

            // compensate delays according to latency
            for (unsigned i = 0; i < count; ++i)
                delays[i] = std::max(0.0f, delays[i] - latency[i]);

            // calculate level
            tapControl.smoothLevelLinear_.nextBlock(level, count);

            // calculate pan
            tapControl.smoothPanNormalized_.nextBlock(pan, count);

            // calculate width (stereo only)
            if (numInputs == 2)
                tapControl.smoothWidth_.nextBlock(width, count);

            // compute FX parameters
            fxControl.filter = tapControl.filterEnable_ ? tapControl.filter_ : GdFilterOff;
            tapControl.smoothLpfCutoff_.nextBlock(fxControl.lpfCutoff, count);
            tapControl.smoothHpfCutoff_.nextBlock(fxControl.hpfCutoff, count);
            tapControl.smoothResonanceLinear_.nextBlock(fxControl.resonance, count);
            tapControl.smoothShiftLinear_.nextBlock(fxControl.shift, count);

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
    simde__m128 x = simde_mm_setr_ps(1.0f - value, value, 0.0f, 0.0f);
    simde__m128 y = simde_mm_sqrt_ps(x);
    return y;
}

static inline simde__m128 calcPanGainsPS(simde__m128 value)
{
    return simde_mm_sqrt_ps(value);
}

//==============================================================================
void GdNetwork::mixMonoToStereo(unsigned tapIndex, const float *input, const float *level, const float *pans, const float *wet, float *const outputs[], unsigned count)
{
    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    unsigned i = 0;

    for (; i + 3 < count; i += 4) {
        simde__m128 pan = simde_mm_load_ps(&pans[i]);
        simde__m128 panGainLeft = calcPanGainsPS(simde_mm_sub_ps(simde_mm_set1_ps(1.0f), pan));
        simde__m128 panGainRight = calcPanGainsPS(pan);

        simde__m128 in = simde_mm_load_ps(&input[i]);
        simde__m128 gain = simde_mm_mul_ps(simde_mm_load_ps(&wet[i]), simde_mm_load_ps(&level[i]));
        simde__m128 leftSample = simde_mm_mul_ps(panGainLeft, simde_mm_mul_ps(in, gain));
        simde__m128 rightSample = simde_mm_mul_ps(panGainRight, simde_mm_mul_ps(in, gain));

        simde_mm_storeu_ps(&leftOutput[i], simde_mm_add_ps(simde_mm_loadu_ps(&leftOutput[i]), leftSample));
        simde_mm_storeu_ps(&rightOutput[i], simde_mm_add_ps(simde_mm_loadu_ps(&rightOutput[i]), rightSample));
    }

    for (; i < count; ++i) {
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
    const float *leftInput = inputs[0];
    const float *rightInput = inputs[1];
    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    unsigned i = 0;

    for (; i + 3 < count; i += 4) {
        simde__m128 pan = simde_mm_load_ps(&pans[i]);
        simde__m128 panGainLeft = calcPanGainsPS(simde_mm_sub_ps(simde_mm_set1_ps(1.0f), pan));
        simde__m128 panGainRight = calcPanGainsPS(pan);

        simde__m128 gain = simde_mm_mul_ps(simde_mm_load_ps(&wet[i]), simde_mm_load_ps(&level[i]));
        simde__m128 leftSample = simde_mm_mul_ps(panGainLeft,
            simde_mm_mul_ps(simde_mm_load_ps(&leftInput[i]), gain));
        simde__m128 rightSample = simde_mm_mul_ps(panGainRight,
            simde_mm_mul_ps(simde_mm_load_ps(&rightInput[i]), gain));

        simde__m128 mid = simde_mm_mul_ps(simde_mm_set1_ps(0.5f), simde_mm_add_ps(leftSample, rightSample));
        simde__m128 side = simde_mm_mul_ps(simde_mm_set1_ps(0.5f), simde_mm_sub_ps(rightSample, leftSample));
        simde__m128 width = simde_mm_load_ps(&widths[i]);
        simde__m128 att = simde_mm_max_ps(simde_mm_add_ps(simde_mm_set1_ps(1.0f), width), simde_mm_set1_ps(2.0f));
        leftSample = simde_mm_div_ps(mid - width * side, att);
        rightSample = simde_mm_div_ps(mid + width * side, att);

        simde_mm_storeu_ps(&leftOutput[i], simde_mm_add_ps(simde_mm_loadu_ps(&leftOutput[i]), leftSample));
        simde_mm_storeu_ps(&rightOutput[i], simde_mm_add_ps(simde_mm_loadu_ps(&rightOutput[i]), rightSample));
    }

    for (; i < count; ++i) {
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
        smoother->setTimeConstant(GdParamSmoothTime);
}

void GdNetwork::TapControl::clear()
{
    for (LinearSmoother *smoother : getSmoothers())
        smoother->clearToTarget();
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
        &smoothPanNormalized_,
        &smoothWidth_,
    }};
}
