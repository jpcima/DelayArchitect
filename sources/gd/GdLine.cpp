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
    std::fill(lineBuffer_.begin(), lineBuffer_.end(), 0.0f);
    lineIndex_ = 0;
}

void GdLine::setSampleRate(float sampleRate)
{
    if (sampleRate_ == sampleRate)
        return;

    sampleRate_ = sampleRate;
    lineCapacity_ = (unsigned)std::ceil(sampleRate * maxDelay_) + kLineSIMDExtra;
    allocateLineBuffer();
}

void GdLine::setMaxDelay(float maxDelay)
{
    if (maxDelay_ == maxDelay)
        return;

    maxDelay_ = maxDelay;
    lineCapacity_ = (unsigned)std::ceil(sampleRate_ * maxDelay) + kLineSIMDExtra;
    allocateLineBuffer();
}

void GdLine::process(const float *input, const float *delay, float *output, unsigned count)
{
    float *lineData = lineData_;
    unsigned lineIndex = lineIndex_;
    unsigned lineCapacity = lineCapacity_;
    unsigned lineCapacityPlusExtra = lineCapacity + kLineCyclicExtra;
    float sampleRate = sampleRate_;

    for (unsigned i = 0; i < count; ++i) {
        float currentInput = input[i];
        lineData[lineIndex] = currentInput;

        ///
        unsigned secondaryLineIndex = lineIndex + lineCapacity;
        secondaryLineIndex = (secondaryLineIndex < lineCapacityPlusExtra) ? secondaryLineIndex : lineIndex;
        lineData[secondaryLineIndex] = currentInput;

        ///
        float sampleDelay = sampleRate * delay[i];
        float fractionalPosition = sampleDelay - (unsigned)sampleDelay;
        unsigned decimalPosition = lineIndex + lineCapacity - (unsigned)sampleDelay;
        decimalPosition -= (decimalPosition < lineCapacity) ? 0 : lineCapacity;

        ///
        unsigned i1 = decimalPosition;
        unsigned i2 = decimalPosition + 1;
        output[i] = lineData[i1] + fractionalPosition * (lineData[i2] - lineData[i1]);

        ///
        lineIndex = (lineIndex + 1 < lineCapacity) ? (lineIndex + 1) : 0;
    }

    ///
    lineIndex_ = lineIndex;
}

void GdLine::allocateLineBuffer()
{
    lineBuffer_.clear();
    lineBuffer_.resize((size_t)lineCapacity_ + kLineCyclicExtra + kLineSIMDExtra);
    lineData_ = lineBuffer_.data() + kLineSIMDExtra;
}
