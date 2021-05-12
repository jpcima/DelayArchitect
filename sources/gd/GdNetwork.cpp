// SPDX-License-Identifier: AGPL-3.0-or-later
#include "GdNetwork.h"
#include <algorithm>
#include <cstdio>
#include <cmath>

GdNetwork::GdNetwork()
{
    smoothFbGain_.setSmoothTime(GdParamSmoothTime);
}

GdNetwork::~GdNetwork()
{
}

void GdNetwork::setChannelMode(ChannelMode channelMode)
{
    channelMode_ = channelMode;
}

void GdNetwork::clear()
{
    smoothFbGain_.clear(fbTapGain_);

    feedback_ = 0;

    for (Tap &tap : taps_)
        tap.clear();
}

void GdNetwork::setSampleRate(float sampleRate)
{
    smoothFbGain_.setSampleRate(sampleRate);

    for (Tap &tap : taps_)
        tap.setSampleRate(sampleRate);
}

void GdNetwork::setBufferSize(unsigned bufferSize)
{
    for (std::vector<float> &temp : temp_)
        temp.resize(bufferSize);

    for (Tap &tap : taps_)
        tap.setBufferSize(bufferSize);
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

        Tap &tap = taps_[tapIndex];

        switch (parameter - tapIndex * numTapParameters) {
        case GDP_TAP_A_ENABLE:
            if (!value)
                tap.enable_ = false;
            else if (!tap.enable_) {
                tap.enable_ = true;
                tap.clear();
            }
            break;
        case GDP_TAP_A_DELAY:
            tap.delay_ = value;
            break;
        }
    }
}

void GdNetwork::process(const float *input, const float *dry, const float *wet, float *outputs[], unsigned count)
{
    unsigned fbTapIndex = fbTapIndex_;
    float feedback = feedback_;
    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];
    float *delays = temp_[0].data();
    float *feedbackGain = temp_[1].data();
    float *feedbackTapOutput = temp_[2].data();
    float *ordinaryTapOutput = temp_[3].data();
    float *inputAndFeedbackSum = temp_[4].data();
    const float *tapInput = input;

    // fill outputs with dry signal
    switch (channelMode_) {
        case ChannelMode::Mono:
            for (unsigned i = 0; i < count; ++i) {
                leftOutput[i] += dry[i] * input[i];
                rightOutput[i] += dry[i] * input[i];
            }
            break;
        case ChannelMode::StereoLeft:
            for (unsigned i = 0; i < count; ++i) {
                leftOutput[i] += dry[i] * input[i];
            }
            break;
        case ChannelMode::StereoRight:
            for (unsigned i = 0; i < count; ++i) {
                rightOutput[i] += dry[i] * input[i];
            }
            break;
    }

    // skip processing the feedback if disabled
    if (fbTapGain_ <= GdMinGain && smoothFbGain_.current() <= GdMinGain) {
        fbTapIndex = ~0u;
    }

    // if there is a feedback line, process it first
    if (fbTapIndex == ~0u) {
        std::fill_n(feedbackTapOutput, count, 0.0f);
    }
    else {
        Tap &tap = taps_[fbTapIndex];

        if (tap.enable_) {
            // compute the line delays
            std::fill_n(delays, count, tap.delay_);
            tap.smoothDelay_.process(delays, delays, count, true);

            // compute the feedback gain
            std::fill_n(feedbackGain, count, fbTapGain_);
            smoothFbGain_.process(feedbackGain, feedbackGain, count, true);

            // compute the line and its effects
            for (unsigned i = 0; i < count; ++i) {
                float in = input[i] + feedback * feedbackGain[i];
                inputAndFeedbackSum[i] = in;

                float out = tap.line_.processOne(in, delays[i]);
                out = tap.fx_.processOne(out);

                feedbackTapOutput[i] = out;
                feedback = out;
            }

            // use this as input to the rest of taps
            tapInput = inputAndFeedbackSum;
        }
    }

    for (unsigned tapIndex = 0; tapIndex < GdMaxLines; ++tapIndex) {
        Tap &tap = taps_[tapIndex];

        if (!tap.enable_)
            continue;

        if (tapIndex == fbTapIndex) {
            // add to stereo mix
            mixToStereoAdding(tapIndex, feedbackTapOutput, wet, outputs, count);
        }
        else {
            // compute the line delays
            std::fill_n(delays, count, tap.delay_);
            tap.smoothDelay_.process(delays, delays, count, true);

            // compute the line and its effects
            tap.line_.process(tapInput, delays, ordinaryTapOutput, count);
            tap.fx_.process(ordinaryTapOutput, ordinaryTapOutput, count);

            // add to stereo mix
            mixToStereoAdding(tapIndex, ordinaryTapOutput, wet, outputs, count);
        }
    }

    feedback_ = feedback;
}

//==============================================================================
void GdNetwork::mixToStereoAdding(unsigned tapIndex, const float *input, const float *wet, float *outputs[], unsigned count)
{
    // TODO pan, level

    float *leftOutput = outputs[0];
    float *rightOutput = outputs[1];

    switch (channelMode_) {
        case ChannelMode::Mono:
            for (unsigned i = 0; i < count; ++i) {
                float in = input[i];
                leftOutput[i] += wet[i] * in;
                rightOutput[i] += wet[i] * in;
            }
            break;
        case ChannelMode::StereoLeft:
            for (unsigned i = 0; i < count; ++i) {
                leftOutput[i] += wet[i] * input[i];
            }
            break;
        case ChannelMode::StereoRight:
            for (unsigned i = 0; i < count; ++i) {
                rightOutput[i] += wet[i] * input[i];
            }
            break;
    }
}

//==============================================================================
GdNetwork::Tap::Tap()
{
    smoothDelay_.setSmoothTime(GdParamSmoothTime);
}

void GdNetwork::Tap::clear()
{
    smoothDelay_.clear(delay_);
    line_.clear();
    fx_.clear();
}

void GdNetwork::Tap::setSampleRate(float sampleRate)
{
    smoothDelay_.setSampleRate(sampleRate);
    line_.setSampleRate(sampleRate);
    fx_.setSampleRate(sampleRate);
}

void GdNetwork::Tap::setBufferSize(unsigned bufferSize)
{
    line_.setBufferSize(bufferSize);
    fx_.setBufferSize(bufferSize);
}
