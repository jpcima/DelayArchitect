// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

class GdTapFx {
public:
    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize);
    void process(const float *input, float *output, unsigned count);
    float processOne(float input);
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

inline void GdTapFx::process(const float *input, float *output, unsigned count)
{
    // TODO
    for (unsigned i = 0; i < count; ++i) {
        output[i] = input[i];
    }
}

inline float GdTapFx::processOne(float input)
{
    // TODO
    return input;
}
