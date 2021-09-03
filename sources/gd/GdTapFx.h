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
#include "GdFilter.h"
#include "GdShifter.h"
#include "GdDefs.h"
#include "filters/GdFilterAA.h"
#include <cstdio>

#define GD_SHIFTER_USES_AA_FILTER 1

class GdTapFx {
public:
    struct Control;

    enum { kControlUpdateInterval = 16 };

    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void performKRateUpdates(Control control, unsigned index);
    void process(const float *input, float *output, Control control, unsigned count);
    float processOne(float input, Control control, unsigned index);
    float getLatency() const;

    struct Control {
        int filter = GdFilterOff;
        float *lpfCutoff = nullptr;
        float *hpfCutoff = nullptr;
        float *resonance = nullptr;
        float *shift = nullptr;
    };

    GdFilter lpf_;
    GdFilter hpf_;
#if GD_SHIFTER_USES_AA_FILTER
    GdFilterAA shifterAA_;
#endif
    GdShifter shifter_;
};

//==============================================================================
inline void GdTapFx::clear()
{
    lpf_.clear();
    hpf_.clear();
#if GD_SHIFTER_USES_AA_FILTER
    shifterAA_.clear();
#endif
    shifter_.clear();
}

inline void GdTapFx::setSampleRate(float sampleRate)
{
    lpf_.setSampleRate(sampleRate);
    hpf_.setSampleRate(sampleRate);
#if GD_SHIFTER_USES_AA_FILTER
    shifterAA_.setSampleRate(sampleRate);
#endif
    shifter_.setSampleRate(sampleRate);
    clear();
}

inline void GdTapFx::setBufferSize(unsigned bufferSize)
{
    shifter_.setBufferSize(bufferSize);
}

inline void GdTapFx::performKRateUpdates(Control control, unsigned index)
{
    {
        int filter[2];
        float cutoff[2];
        float resonance;
        GdFilter *filters[2] = {&lpf_, &hpf_};

        switch (control.filter) {
        default:
            filter[0] = GdFilter::kFilterOff;
            filter[1] = GdFilter::kFilterOff;
            break;
        case GdFilter6dB:
            filter[0] = GdFilter::kFilterLPF6;
            filter[1] = GdFilter::kFilterHPF6;
            break;
        case GdFilter12dB:
            filter[0] = GdFilter::kFilterLPF12;
            filter[1] = GdFilter::kFilterHPF12;
            break;
        }
        cutoff[0] = control.lpfCutoff[index];
        cutoff[1] = control.hpfCutoff[index];
        resonance = control.resonance[index];

        for (unsigned i = 0; i < 2; ++i) {
            GdFilter &f = *filters[i];
            bool mustUpdate = false;
            if (f.getFilterType() != filter[i]) {
                f.setFilterType(filter[i]);
                f.clear();
                mustUpdate = true;
            }
            if (f.getCutoff() != cutoff[i]) {
                f.setCutoff(cutoff[i]);
                mustUpdate = true;
            }
            if (f.getResonance() != resonance) {
                f.setResonance(resonance);
                mustUpdate = true;
            }
            if (mustUpdate)
                f.updateCoeffs();
        }
    }

#if GD_SHIFTER_USES_AA_FILTER
    {
        GdFilterAA &shifterAA = shifterAA_;
        shifterAA.setCutoff(shifterAA.getSampleRate() / (2 * control.shift[index]));
    }
#endif

#if GD_SHIFTER_UPDATES_AT_K_RATE
    {
        GdShifter &shifter = shifter_;
        shifter.setShift(control.shift[index]);
    }
#endif
}

inline void GdTapFx::process(const float *input, float *output, Control control, unsigned count)
{
    {
        GdFilter &lpf = lpf_;
        lpf.process(input, output, count);
    }

    input = output;

    {
        GdFilter &hpf = hpf_;
        hpf.process(input, output, count);
    }

    input = output;

#if GD_SHIFTER_USES_AA_FILTER
    {
        GdFilterAA &shifterAA = shifterAA_;
        shifterAA.process(input, output, count);
    }

    input = output;
#endif

    {
        GdShifter &shifter = shifter_;
#if GD_SHIFTER_UPDATES_AT_K_RATE
        shifter.process(input, output, count);
#else
        shifter.process(input, output, control.shift, count);
#endif
    }
}

inline float GdTapFx::processOne(float input, Control control, unsigned index)
{
    float output;

    {
        GdFilter &lpf = lpf_;
        output = lpf.processOne(input);
    }

    input = output;

    {
        GdFilter &hpf = hpf_;
        output = hpf.processOne(input);
    }

    input = output;

#if GD_SHIFTER_USES_AA_FILTER
    {
        GdFilterAA &shifterAA = shifterAA_;
        output = shifterAA.processOne(input);
    }

    input = output;
#endif

    {
        GdShifter &shifter = shifter_;
#if GD_SHIFTER_UPDATES_AT_K_RATE
        output = shifter.processOne(input);
#else
        output = shifter.processOne(input, control.shift[index]);
#endif
    }

    return output;
}

inline float GdTapFx::getLatency() const
{
    float latency;
#if GD_SHIFTER_CAN_REPORT_LATENCY
    const GdShifter &shifter = shifter_;
    latency = shifter.getLatency();
#else
    latency = 0.0f;
#endif
    return latency;
}
