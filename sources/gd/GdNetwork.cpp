// SPDX-License-Identifier: AGPL-3.0-or-later
#include "GdNetwork.h"
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

    smoothFbGain_.setSmoothTime(GdParamSmoothTime);
}

GdNetwork::~GdNetwork()
{
}

void GdNetwork::clear()
{
    smoothFbGain_.clear(fbTapGain_);

    for (ChannelDsp &chan : channels_)
        chan.clear();

    for (TapControl &tapControl : tapControls_)
        tapControl.clear();
}

void GdNetwork::setSampleRate(float sampleRate)
{
    smoothFbGain_.setSampleRate(sampleRate);

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
        case GDP_FEEDBACK_TAP:
            fbTapIndex_ = (unsigned)value;
            break;
        case GDP_FEEDBACK_GAIN:
            fbTapGain_ = value;
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
        }
    }
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

    float *delays = temp_[0].data();
    float *feedbackGain = temp_[1].data();
    float *feedbackTapOutputs[2] = { temp_[2].data(), temp_[3].data() };
    float *ordinaryTapOutputs[2] = { temp_[4].data(), temp_[5].data() };
    float *inputAndFeedbackSums[2] = { temp_[6].data(), temp_[7].data() };

    const float *tapInputs[2] = { leftInput, rightInput };

    // fill outputs with dry signal
    for (unsigned i = 0; i < count; ++i) {
        float gain = dry[i];
        leftOutput[i] = gain * leftInput[i];
        rightOutput[i] = gain * rightInput[i];
    }

    // skip processing the feedback if disabled
    if (fbTapGain_ <= GdMinGain && smoothFbGain_.current() <= GdMinGain) {
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
            std::fill_n(delays, count, tapControl.delay_);
            tapControl.smoothDelay_.process(delays, delays, count, true);

            // compute the feedback gain
            std::fill_n(feedbackGain, count, fbTapGain_);
            smoothFbGain_.process(feedbackGain, feedbackGain, count, true);

            for (unsigned chanIndex = 0; chanIndex < numInputs; ++chanIndex) {
                ChannelDsp &chan = channels_[chanIndex];
                TapDsp &tap = chan.taps_[fbTapIndex];
                float feedback = chan.feedback_;

                // compute the line and its effects
                const float *input = inputs[chanIndex];
                float *inputAndFeedbackSum = inputAndFeedbackSums[chanIndex];
                float *feedbackTapOutput = feedbackTapOutputs[chanIndex];

                for (unsigned i = 0; i < count; ++i) {
                    float in = input[i] + feedback * feedbackGain[i];
                    inputAndFeedbackSum[i] = in;

                    float out = tap.line_.processOne(in, delays[i]);
                    out = tap.fx_.processOne(out);

                    feedbackTapOutput[i] = out;
                    feedback = out;
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
                mixStereoToStereo(tapIndex, feedbackTapOutputs, wet, outputs, count);
            else
                mixMonoToStereo(tapIndex, feedbackTapOutputs[0], wet, outputs, count);
        }
        else {
            // compute the line delays
            std::fill_n(delays, count, tapControl.delay_);
            tapControl.smoothDelay_.process(delays, delays, count, true);

            for (unsigned chanIndex = 0; chanIndex < numInputs; ++chanIndex) {
                ChannelDsp &chan = channels_[chanIndex];
                TapDsp &tap = chan.taps_[tapIndex];

                // compute the line and its effects
                const float *tapInput = tapInputs[chanIndex];
                float *ordinaryTapOutput = ordinaryTapOutputs[chanIndex];

                tap.line_.process(tapInput, delays, ordinaryTapOutput, count);
                tap.fx_.process(ordinaryTapOutput, ordinaryTapOutput, count);
            }

            // add to stereo mix
            if (numInputs == 2)
                mixStereoToStereo(tapIndex, ordinaryTapOutputs, wet, outputs, count);
            else
                mixMonoToStereo(tapIndex, ordinaryTapOutputs[0], wet, outputs, count);
        }
    }
}

//==============================================================================
void GdNetwork::mixMonoToStereo(unsigned tapIndex, const float *input, const float *wet, float *const outputs[], unsigned count)
{
    // TODO pan, level

    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    for (unsigned i = 0; i < count; ++i) {
        float gain = wet[i];
        float in = input[i];
        leftOutput[i] += gain * in;
        rightOutput[i] += gain * in;
    }
}

void GdNetwork::mixStereoToStereo(unsigned tapIndex, const float *const inputs[], const float *wet, float *const outputs[], unsigned count)
{
    // TODO pan, spread, level

    const float *leftInput = inputs[0];
    const float *rightInput = inputs[1];
    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    for (unsigned i = 0; i < count; ++i) {
        float gain = wet[i];
        leftOutput[i] += gain * leftInput[i];
        rightOutput[i] += gain * rightInput[i];
    }
}

//==============================================================================
GdNetwork::TapDsp::TapDsp()
{
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
    line_.setBufferSize(bufferSize);
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
    smoothDelay_.setSmoothTime(GdParamSmoothTime);
}

void GdNetwork::TapControl::clear()
{
    smoothDelay_.clear(delay_);
}

void GdNetwork::TapControl::setSampleRate(float sampleRate)
{
    smoothDelay_.setSampleRate(sampleRate);
}
