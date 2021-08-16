// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "GdDefs.h"

class GdTapFx {
public:
    struct Control;

    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void process(const float *input, float *output, Control control, unsigned count);
    float processOne(float input, Control control, unsigned index);

    struct Control {
        int filter = GdFilterOff;
        float *lpfCutoff = nullptr;
        float *hpfCutoff = nullptr;
        float *resonance = nullptr;
    };
};

//==============================================================================
inline void GdTapFx::clear()
{
    // TODO
}

inline void GdTapFx::setSampleRate(float sampleRate)
{
    // TODO
    (void)sampleRate;
}

inline void GdTapFx::setBufferSize(unsigned bufferSize)
{
    // TODO
    (void)bufferSize;
}

inline void GdTapFx::process(const float *input, float *output, Control control, unsigned count)
{
    // TODO
    for (unsigned i = 0; i < count; ++i) {
        output[i] = input[i];
    }
}

inline float GdTapFx::processOne(float input, Control control, unsigned index)
{
    // TODO
    return input;
}
