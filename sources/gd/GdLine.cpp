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
#include "GdSIMD.h"
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

    unsigned i = 0;

#if 1
    for (; i + 3 < count; i += 4) {
        simde__m128 currentInputPS = simde_mm_loadu_ps(&input[i]);

        int writeIndex1 = (int)lineIndex;
        int writeIndex2 = writeIndex1;
        writeIndex2 -= (writeIndex1 > (int)(lineCapacityPlusExtra - 4)) ? (int)lineCapacity : 0;
        writeIndex2 += (writeIndex1 < kLineCyclicExtra) ? (int)lineCapacity : 0;

        simde_mm_storeu_ps(&lineData[writeIndex1], currentInputPS);
        simde_mm_storeu_ps(&lineData[writeIndex2], currentInputPS);

        ///
        simde__m128i lineCapacityI = simde_mm_set1_epi32((int)lineCapacity);
        simde__m128i lineIndexI = simde_mm_add_epi32(simde_mm_set1_epi32(lineIndex), simde_mm_setr_epi32(0, 1, 2, 3));
        //lineIndexI = simde_mm_sub_epi32(lineIndexI,
        //    ternaryEPI32(simde_mm_cmplt_epi32(lineIndexI, lineCapacityI), simde_mm_set1_epi32(0), lineCapacityI));

        simde__m128 sampleDelayPS = simde_mm_mul_ps(simde_mm_set1_ps(sampleRate), simde_mm_load_ps(&delay[i]));
        simde__m128i sampleDelayI = simde_mm_cvtps_epi32(sampleDelayPS);
        simde__m128 fractionalPositionPS = simde_mm_sub_ps(sampleDelayPS, simde_mm_cvtepi32_ps(sampleDelayI));
        simde__m128i decimalPositionI = simde_mm_sub_epi32(simde_mm_add_epi32(lineIndexI, lineCapacityI), sampleDelayI);
        decimalPositionI = simde_mm_sub_epi32(decimalPositionI,
            ternaryEPI32(simde_mm_cmplt_epi32(decimalPositionI, lineCapacityI), simde_mm_set1_epi32(0), lineCapacityI));

        ///
        simde_mm_storeu_ps(&output[i], lerpPS(lineData, decimalPositionI, fractionalPositionPS));

        ///
        lineIndex += 4;
        lineIndex -= (lineIndex < lineCapacity) ? 0 : lineCapacity;
    }
#endif

    for (; i < count; ++i) {
        float currentInput = input[i];

        unsigned writeIndex1 = lineIndex;
        unsigned writeIndex2 = lineIndex + lineCapacity;
        writeIndex2 = (writeIndex2 < lineCapacityPlusExtra) ? writeIndex2 : lineIndex;
        lineData[writeIndex1] = currentInput;
        lineData[writeIndex2] = currentInput;

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
        lineIndex += 1;
        lineIndex -= (lineIndex < lineCapacity) ? 0 : lineCapacity;
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
