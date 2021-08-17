// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "GdFilter.h"
#include "GdDefs.h"
#include <cstdio>

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

    struct Control {
        int filter = GdFilterOff;
        float *lpfCutoff = nullptr;
        float *hpfCutoff = nullptr;
        float *resonance = nullptr;
    };

    GdFilter lpf_;
    GdFilter hpf_;
};

//==============================================================================
inline void GdTapFx::clear()
{
    // TODO
    lpf_.clear();
    hpf_.clear();
}

inline void GdTapFx::setSampleRate(float sampleRate)
{
    // TODO
    lpf_.setSampleRate(sampleRate);
    hpf_.setSampleRate(sampleRate);
    clear();
}

inline void GdTapFx::setBufferSize(unsigned bufferSize)
{
    // TODO
    (void)bufferSize;
}

inline void GdTapFx::performKRateUpdates(Control control, unsigned index)
{
    // TODO
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
}

inline void GdTapFx::process(const float *input, float *output, Control control, unsigned count)
{
    // TODO
    {
        GdFilter &lpf = lpf_;
        for (unsigned i = 0; i < count; ++i)
            output[i] = lpf.processOne(input[i]);
    }

    input = output;

    {
        GdFilter &hpf = hpf_;
        for (unsigned i = 0; i < count; ++i)
            output[i] = hpf.processOne(input[i]);
    }
}

inline float GdTapFx::processOne(float input, Control control, unsigned index)
{
    // TODO
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

    return output;
}
