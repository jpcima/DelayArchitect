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

#include "GdLine.h"
#include <algorithm>
#include <cstdio>
#include <cmath>

void GdLine::clear()
{
    lastOutput_ = 0;
    std::fill(lineData_.begin(), lineData_.end(), 0.0f);
    lineIndex_ = 0;
}

void GdLine::setSampleRate(float sampleRate)
{
    if (sampleRate_ == sampleRate)
        return;

    sampleRate_ = sampleRate;
    allocateLineBuffer((unsigned)std::ceil(sampleRate * maxDelay_));
}

void GdLine::setMaxDelay(float maxDelay)
{
    if (maxDelay_ == maxDelay)
        return;

    maxDelay_ = maxDelay;
    allocateLineBuffer((unsigned)std::ceil(sampleRate_ * maxDelay));
}

void GdLine::process(const float *input, const float *delay, const float *diffusion, float *output, unsigned count)
{
    float lastOutput = lastOutput_;
    float *lineData = lineData_.data();
    unsigned lineIndex = lineIndex_;
    unsigned lineCapacity = (unsigned)lineData_.size();
    float maxDelay = maxDelay_;
    float sampleRate = sampleRate_;

    for (unsigned i = 0; i < count; ++i) {
        float in = input[i];
        float diff = diffusion[i];
        lineData[lineIndex] = in + diff * lastOutput;

        ///
        float sampleDelay = sampleRate * delay[i];
        float fractionalPosition = sampleDelay - (unsigned)sampleDelay;
        unsigned decimalPosition = lineIndex + lineCapacity - (unsigned)sampleDelay;
        decimalPosition -= (decimalPosition < lineCapacity) ? 0 : lineCapacity;

        ///
        unsigned i1 = decimalPosition;
        unsigned i2 = decimalPosition + 1;
        i2 = (i2 < lineCapacity) ? i2 : 0;
        float lineOutput = lineData[i1] + fractionalPosition * (lineData[i2] - lineData[i1]);

        ///
        lastOutput = lineOutput - in * diff;
        output[i] = lastOutput;

        ///
        lineIndex = (lineIndex + 1 < lineCapacity) ? (lineIndex + 1) : 0;
    }

    ///
    lineIndex_ = lineIndex;
    lastOutput_ = lastOutput;
}

void GdLine::allocateLineBuffer(unsigned capacity)
{
    lineData_.clear();
    lineData_.resize(capacity);
}
