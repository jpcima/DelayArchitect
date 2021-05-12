// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "GdLine.h"
#include "GdTapFx.h"
#include "GdDefs.h"
#include "utility/LinearSmoother.h"
#include <vector>

class GdNetwork {
public:
    enum ChannelMode {
        Mono,
        StereoLeft,
        StereoRight,
    };

    GdNetwork();
    ~GdNetwork();
    void setChannelMode(ChannelMode channelMode);
    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void setParameter(unsigned parameter, float value);
    void process(const float *input, const float *dry, const float *wet, float *outputs[], unsigned count);

//==============================================================================
private:
    void mixToStereoAdding(unsigned tapIndex, const float *input, const float *wet, float *outputs[], unsigned count);

//==============================================================================
private:
    struct Tap {
        Tap();
        void clear();
        void setSampleRate(float sampleRate);
        void setBufferSize(unsigned bufferSize);

        // parameters
        bool enable_ = false;
        float delay_;

        // smoothers
        LinearSmoother smoothDelay_;

        // parts
        GdLine line_;
        GdTapFx fx_;
    };

    // channel
    ChannelMode channelMode_ = ChannelMode::Mono;

    // parameters
    unsigned fbTapIndex_ = 0;
    float fbTapGain_ = GdMinGain;

    // smoothers
    LinearSmoother smoothFbGain_;

    // internal
    float feedback_ = 0;
    std::vector<float> temp_[5];

    // taps
    Tap taps_[GdMaxLines];
};
