// SPDX-License-Identifier: AGPL-3.0-or-later
#include "GdLine.h"
#include <algorithm>
#include <cstdio>
#include <cmath>

void GdLine::clear()
{
    std::fill(lineData_.begin(), lineData_.end(), 0.0f);
    lineIndex_ = 0;
}

void GdLine::setSampleRate(float sampleRate)
{
    if (sampleRate_ == sampleRate)
        return;

    sampleRate_ = sampleRate;

    lineData_.clear();
    lineData_.resize((unsigned)std::ceil(sampleRate * GdMaxDelay));
}

void GdLine::process(const float *input, const float *delay, float *output, unsigned count)
{
    float *lineData = lineData_.data();
    unsigned lineIndex = lineIndex_;
    unsigned lineCapacity = (unsigned)lineData_.capacity();
    float sampleRate = sampleRate_;

    for (unsigned i = 0; i < count; ++i) {
        lineData[lineIndex] = input[i];

        ///
        float limitedDelay = delay[i];
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
        output[i] = lineData[i1] + fractionalPosition * (lineData[i2] - lineData[i1]);

        ///
        lineIndex = (lineIndex + 1 < lineCapacity) ? (lineIndex + 1) : 0;
    }

    ///
    lineIndex_ = lineIndex;
}
