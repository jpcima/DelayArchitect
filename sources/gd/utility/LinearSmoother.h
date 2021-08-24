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

#pragma once
#include <utility>
#include <cmath>
#include <cstdint>
#include <simde/hedley.h>
#include <simde/simde-features.h>
#if SIMDE_NATURAL_VECTOR_SIZE_GE(128)
#   include <simde/x86/sse.h>
#endif

/**
 * @brief A linear smoother for control values
 *
 * This continually smooths a value towards a defined target,
 * using linear segments.
 *
 * The duration of the smoothing segment is defined by the given
 * time constant. Every time the target changes, a new segment
 * restarts for the whole duration of the time constant.
 *
 * Note that this smoother, unlike an exponential smoother,
 * eventually should converge to its target value.
 */
class LinearSmoother {
public:
    void setSampleRate(float newSampleRate) noexcept;
    void setTimeConstant(float newTau) noexcept;
    float getCurrentValue() const noexcept;
    float getTarget() const noexcept;
    void setTarget(float newTarget) noexcept;
    void clear() noexcept;
    void clearToTarget() noexcept;
    float next() noexcept;
#if SIMDE_NATURAL_VECTOR_SIZE_GE(128)
    simde__m128 nextPS() noexcept;
#endif
    void nextBlock(float *__restrict output, uint32_t count) noexcept;

private:
    void updateStep() noexcept;

private:
    float fStep = 0.0f;
    float fTarget = 0.0f;
    float fMem = 0.0f;
    float fTau = 0.0f;
    float fSampleRate = 0.0f;
};

HEDLEY_ALWAYS_INLINE float LinearSmoother::getCurrentValue() const noexcept
{
    return fMem;
}

HEDLEY_ALWAYS_INLINE float LinearSmoother::getTarget() const noexcept
{
    return fTarget;
}

HEDLEY_ALWAYS_INLINE void LinearSmoother::clear() noexcept
{
    fMem = 0.0f;
}

HEDLEY_ALWAYS_INLINE void LinearSmoother::clearToTarget() noexcept
{
    fMem = fTarget;
}

HEDLEY_ALWAYS_INLINE float LinearSmoother::next() noexcept
{
    float y0 = fMem;
    float dy = fTarget - y0;
    return (fMem = y0 + std::copysign(std::fmin(std::abs(dy), std::abs(fStep)), dy));
}

#if SIMDE_NATURAL_VECTOR_SIZE_GE(128)
HEDLEY_ALWAYS_INLINE simde__m128 LinearSmoother::nextPS() noexcept
{
    float y0 = fMem;
    simde__m128 y0PS = simde_mm_set1_ps(y0);
    simde__m128 dyPS = simde_mm_set1_ps(fTarget - y0);
    simde__m128 stepPS = simde_mm_mul_ps(simde_mm_set1_ps(fStep), simde_mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f));
    auto copySignPS = [](simde__m128 x, simde__m128 y) {
        simde__m128 maskPS = simde_mm_set1_ps(-0.0f);
        return simde_mm_or_ps(simde_mm_andnot_ps(maskPS, x), simde_mm_and_ps(maskPS, y));
    };
    simde__m128 signMaskPS = simde_mm_set1_ps(-0.0f);
    simde__m128 outPS = simde_mm_add_ps(y0PS, copySignPS(simde_mm_min_ps(simde_x_mm_abs_ps(dyPS), simde_x_mm_abs_ps(stepPS)), dyPS));
    fMem = ((float *)&outPS)[3];
    return outPS;
}
#endif
