// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <vector>

class GdLine {
public:
    void clear();
    void setSampleRate(float sampleRate);
    void process(const float *input, const float *delay, float *output, unsigned count);
    float processOne(float input, float delay);

private:
    std::vector<float> lineData_;
    unsigned lineIndex_ = 0;
    float sampleRate_ = 0;
};

//==============================================================================
#include "GdDefs.h"

inline float GdLine::processOne(float input, float delay)
{
    float *lineData = lineData_.data();
    unsigned lineIndex = lineIndex_;
    unsigned lineCapacity = (unsigned)lineData_.capacity();
    float sampleRate = sampleRate_;

    ///
    lineData[lineIndex] = input;

    ///
    float limitedDelay = delay;
    limitedDelay = (limitedDelay < 0.0f) ? 0.0f : limitedDelay;
    limitedDelay = (limitedDelay > GdMaxDelay) ? GdMaxDelay : limitedDelay;
    float sampleDelay = sampleRate * limitedDelay;

    ///
    float fractionalPosition = sampleDelay - (unsigned)sampleDelay;
    unsigned decimalPosition = lineIndex + lineCapacity - (unsigned)sampleDelay;
    decimalPosition = (decimalPosition < lineCapacity) ? decimalPosition : (decimalPosition - lineCapacity);

    ///
    unsigned i1 = decimalPosition;
    unsigned i2 = decimalPosition + 1;
    i2 = (i2 < lineCapacity) ? i2 : 0;
    float output = lineData[i1] + fractionalPosition * (lineData[i2] - lineData[i1]);

    ///
    lineIndex = (lineIndex + 1 < lineCapacity) ? (lineIndex + 1) : 0;
    lineIndex_ = lineIndex;

    ///
    return output;
}
