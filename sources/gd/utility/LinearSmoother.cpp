/*
 * Linear smoother
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * SPDX-License-Identifier: ISC
 */

#include "LinearSmoother.h"

void LinearSmoother::setSampleRate(float newSampleRate) noexcept
{
    if (fSampleRate != newSampleRate) {
        fSampleRate = newSampleRate;
        updateStep();
    }
}

void LinearSmoother::setTimeConstant(float newTau) noexcept
{
    if (fTau != newTau) {
        fTau = newTau;
        updateStep();
    }
}

void LinearSmoother::setTarget(float newTarget) noexcept
{
    if (fTarget != newTarget) {
        fTarget = newTarget;
        updateStep();
    }
}

void LinearSmoother::nextBlock(float *__restrict output, uint32_t count) noexcept
{
    float target = fTarget;
    if (fMem == target) {
        for (uint32_t i = 0; i < count; ++i)
            output[i] = target;
        return;
    }

    uint32_t i = 0;
#if SIMDE_NATURAL_VECTOR_SIZE_GE(128)
    for (; i + 4 < count; i += 4)
        _mm_storeu_ps(&output[i], nextPS());
#endif
    for (; i < count; ++i)
        output[i] = next();
}

void LinearSmoother::updateStep() noexcept
{
    fStep = (fTarget - fMem) / (fTau * fSampleRate);
}
