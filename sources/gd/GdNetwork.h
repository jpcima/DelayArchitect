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

#pragma once
#include "GdLine.h"
#include "GdTapFx.h"
#include "GdDefs.h"
#include "utility/LinearSmoother.h"
#include <jsl/allocator>
#include <array>
#include <vector>
#include <memory>

#if !defined(GD_SHIFTER_CAN_REPORT_LATENCY)
#   error Must define GD_SHIFTER_CAN_REPORT_LATENCY
#endif

class GdNetwork {
public:
    enum ChannelMode {
        Mono,
        Stereo,
    };

    explicit GdNetwork(ChannelMode channelMode);
    ~GdNetwork();
    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void setParameter(unsigned parameter, float value);
    void setTempo(float tempo);
    void process(const float *const inputs[], const float *dry, const float *wet, float *const outputs[], unsigned count);

//==============================================================================
private:
    void mixMonoToStereo(unsigned tapIndex, const float *input, const float *level, const float *pan, const float *wet, float *const outputs[], unsigned count);
    void mixStereoToStereo(unsigned tapIndex, const float *const inputs[], const float *level, const float *pan, const float *width, const float *wet, float *const outputs[], unsigned count);

//==============================================================================
private:
    struct TapDsp {
        TapDsp();
        void clear();
        void setSampleRate(float sampleRate);
        void setBufferSize(unsigned bufferSize);

        // parts
        GdLine line_;
        GdTapFx fx_;
    };

    struct ChannelDsp {
        void clear();
        void setSampleRate(float sampleRate);
        void setBufferSize(unsigned bufferSize);

        // internal
        float feedback_ = 0;

        // taps
        TapDsp taps_[GdMaxLines];
    };

    // channels
    std::vector<ChannelDsp> channels_;

    // timing information
    float bpm_ = 120.0f;

    // parameters + smoothers
    bool sync_ = false;
    int div_ = GdDefaultDivisor;
    float swing_ = 0.5f;
    bool fbEnable_ = false;
    unsigned fbTapIndex_ = 0;
    float fbTapGainDB_ = GdMinFeedbackGainDB;
    LinearSmoother smoothFbGainLinear_;

    struct TapControl {
        TapControl();
        void clear();
        void setSampleRate(float sampleRate);

        // parameters
        bool enable_ = false;
        float delay_ = 0;
        float levelDB_ = 0;
        bool mute_ = false;
        bool filterEnable_ = false;
        int filter_ = GdFilterOff;
        float lpfCutoff_ = 0;
        float hpfCutoff_ = 0;
        float resonanceDB_ = 0;
        bool filterAnalog_ = false;
        bool shiftEnable_ = false;
        float shift_ = 0;
        float pan_ = 0;
        float width_ = 0;
        bool flip_ = false;
        // smoothers
        LinearSmoother smoothDelay_;
        LinearSmoother smoothLevelLinear_;
        LinearSmoother smoothLpfCutoff_;
        LinearSmoother smoothHpfCutoff_;
        LinearSmoother smoothResonanceLinear_;
        LinearSmoother smoothShiftLinear_;
        LinearSmoother smoothPanNormalized_;
        LinearSmoother smoothWidth_;
        enum { kNumSmoothers = 8 };
        std::array<LinearSmoother *, kNumSmoothers> getSmoothers();
    };

    TapControl tapControls_[GdMaxLines];
#if GD_SHIFTER_CAN_REPORT_LATENCY
    LinearSmoother smoothTapLatency_[GdMaxLines];
#endif

    // internal
    enum { kNumTempBuffers = 16 };
    using TempBuffer = std::vector<float, jsl::aligned_allocator<float, 32>>;
    std::array<TempBuffer, kNumTempBuffers> temp_;
};
