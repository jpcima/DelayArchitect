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
    void process(const float *input, const float *delay, const float *diffusion, float *output, unsigned count);
    float processOne(float input, float delay, float diffusion);

private:
    float lastOutput_ = 0;
    float lastLineOutput_ = 0;
    std::vector<float> lineData_;
    unsigned lineIndex_ = 0;
    float maxDelay_ = 0;
    float sampleRate_ = 0;
    void allocateLineBuffer(unsigned capacity);
};

//==============================================================================
inline float GdLine::processOne(float input, float delay, float diffusion)
{
    float lastOutput = lastOutput_;
    float lastLineOutput = lastLineOutput_;
    float *lineData = lineData_.data();
    unsigned lineIndex = lineIndex_;
    unsigned lineCapacity = (unsigned)lineData_.size();
    float maxDelay = maxDelay_;
    float sampleRate = sampleRate_;

    ///
    //lineData[lineIndex] = input + diffusion * lastOutput;
    float lineInput = input + diffusion * lastLineOutput;
    lineData[lineIndex] = lineInput;

    ///
    float sampleDelay = sampleRate * delay;
    float fractionalPosition = sampleDelay - (unsigned)sampleDelay;
    unsigned decimalPosition = lineIndex + lineCapacity - (unsigned)sampleDelay;
    decimalPosition -= (decimalPosition < lineCapacity) ? 0 : lineCapacity;

    ///
    unsigned i1 = decimalPosition;
    unsigned i2 = decimalPosition + 1;
    i2 = (i2 < lineCapacity) ? i2 : 0;
    float lineOutput = lineData[i1] + fractionalPosition * (lineData[i2] - lineData[i1]);
    lastLineOutput = lineOutput;

    ///
    //lastOutput = lineOutput - input * diffusion;
    lastOutput = lineOutput - lineInput * diffusion;

    ///
    lineIndex = (lineIndex + 1 < lineCapacity) ? (lineIndex + 1) : 0;
    lineIndex_ = lineIndex;
    lastOutput_ = lastOutput;
    lastLineOutput_ = lastLineOutput;

    ///
    return lastOutput;
}
