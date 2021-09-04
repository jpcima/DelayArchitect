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
#include <vector>

class GdLine {
public:
    void clear();
    void setSampleRate(float sampleRate);
    void setMaxDelay(float maxDelay);
    void process(const float *input, const float *delay, float *output, unsigned count);
    float processOne(float input, float delay);

private:
    std::vector<float> lineData_;
    unsigned lineCapacity_ = 0;
    unsigned lineIndex_ = 0;
    float maxDelay_ = 0;
    float sampleRate_ = 0;
    enum {
        // extra amount of frames, so that a newly pushed pack of frames
        // doesn't overwrite the tail right away (4 frames to permit SSE)
        kLineSIMDExtra = 4,
        // extra amount of frames past the ordinary line capacity, which
        // mirrors the data at the beginning of the buffer
        kLineCyclicExtra = 4,
    };
    void allocateLineBuffer();
};

//==============================================================================
inline float GdLine::processOne(float input, float delay)
{
    float *lineData = lineData_.data();
    unsigned lineIndex = lineIndex_;
    unsigned lineCapacity = lineCapacity_;
    unsigned lineCapacityPlusExtra = lineCapacity + kLineCyclicExtra;
    float sampleRate = sampleRate_;

    ///
    lineData[lineIndex] = input;

    ///
    unsigned secondaryLineIndex = lineIndex + lineCapacity;
    secondaryLineIndex = (secondaryLineIndex < lineCapacityPlusExtra) ? secondaryLineIndex : lineIndex;
    lineData[secondaryLineIndex] = input;

    ///
    float sampleDelay = sampleRate * delay;
    float fractionalPosition = sampleDelay - (unsigned)sampleDelay;
    unsigned decimalPosition = lineIndex + lineCapacity - (unsigned)sampleDelay;
    decimalPosition -= (decimalPosition < lineCapacity) ? 0 : lineCapacity;

    ///
    unsigned i1 = decimalPosition;
    unsigned i2 = decimalPosition + 1;
    float output = lineData[i1] + fractionalPosition * (lineData[i2] - lineData[i1]);

    ///
    lineIndex = (lineIndex + 1 < lineCapacity) ? (lineIndex + 1) : 0;
    lineIndex_ = lineIndex;

    ///
    return output;
}
