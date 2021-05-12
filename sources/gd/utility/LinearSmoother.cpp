// SPDX-License-Identifier: BSD-2-Clause

// This code is part of the sfizz library and is licensed under a BSD 2-clause
// license. You should have receive a LICENSE.md file along with the code.
// If not, contact the sfizz maintainers at https://github.com/sfztools/sfizz

#include "LinearSmoother.h"
#include <simde/simde-features.h>
#if SIMDE_NATURAL_VECTOR_SIZE_GE(128)
#include <simde/x86/sse.h>
#endif
#include <algorithm>

void LinearSmoother::setSampleRate(float sampleRate)
{
    sampleRate_ = sampleRate;
    updateParameters();
}

void LinearSmoother::setSmoothTime(float smoothTime)
{
    smoothTime_ = smoothTime;
    updateParameters();
}

void LinearSmoother::clear(float value)
{
    current_ = value;
    target_ = value;
    step_ = 0.0;
}

void LinearSmoother::clearToTarget()
{
    clear(target_);
}

void LinearSmoother::process(const float *input, float *output, unsigned count, bool canShortcut)
{
    if (count == 0)
        return;

    unsigned i = 0;
    float current = current_;
    float target = target_;
    const int smoothFrames = smoothFrames_;

    if (smoothFrames < 2 || (canShortcut && current == target && current == input[0])) {
        if (input != output)
            std::copy_n(input, count, output);
        clear(input[count - 1]);
        return;
    }

    float step = step_;

#if SIMDE_NATURAL_VECTOR_SIZE_GE(128)
    for (; i + 15 < count; i += 16) {
        const float nextTarget = input[i + 15];
        if (target != nextTarget) {
            target = nextTarget;
            step = (target - current) / std::max(16, smoothFrames);
        }
        const simde__m128 targetX4 = simde_mm_set1_ps(target);
        if (target > current) {
            simde__m128 stepX4 = simde_mm_set1_ps(step);
            simde__m128 tmp1X4 = simde_mm_mul_ps(stepX4, simde_mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f));
            simde__m128 tmp2X4 = simde_mm_shuffle_ps(tmp1X4, tmp1X4, SIMDE_MM_SHUFFLE(3, 3, 3, 3));
            simde__m128 current1X4 = simde_mm_add_ps(simde_mm_set1_ps(current), tmp1X4);
            simde_mm_storeu_ps(&output[i], simde_mm_min_ps(current1X4, targetX4));
            simde__m128 current2X4 = simde_mm_add_ps(current1X4, tmp2X4);
            simde_mm_storeu_ps(&output[i + 4], simde_mm_min_ps(current2X4, targetX4));
            simde__m128 current3X4 = simde_mm_add_ps(current2X4, tmp2X4);
            simde_mm_storeu_ps(&output[i + 8], simde_mm_min_ps(current3X4, targetX4));
            simde__m128 current4X4 = simde_mm_add_ps(current3X4, tmp2X4);
            simde__m128 limited4X4 = simde_mm_min_ps(current4X4, targetX4);
            simde_mm_storeu_ps(&output[i + 12], limited4X4);
            current = simde_mm_cvtss_f32(simde_mm_shuffle_ps(limited4X4, limited4X4, SIMDE_MM_SHUFFLE(3, 3, 3, 3)));
        }
        else if (target < current) {
            simde__m128 stepX4 = simde_mm_set1_ps(step);
            simde__m128 tmp1X4 = simde_mm_mul_ps(stepX4, simde_mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f));
            simde__m128 tmp2X4 = simde_mm_shuffle_ps(tmp1X4, tmp1X4, SIMDE_MM_SHUFFLE(3, 3, 3, 3));
            simde__m128 current1X4 = simde_mm_add_ps(simde_mm_set1_ps(current), tmp1X4);
            simde_mm_storeu_ps(&output[i], simde_mm_max_ps(current1X4, targetX4));
            simde__m128 current2X4 = simde_mm_add_ps(current1X4, tmp2X4);
            simde_mm_storeu_ps(&output[i + 4], simde_mm_max_ps(current2X4, targetX4));
            simde__m128 current3X4 = simde_mm_add_ps(current2X4, tmp2X4);
            simde_mm_storeu_ps(&output[i + 8], simde_mm_max_ps(current3X4, targetX4));
            simde__m128 current4X4 = simde_mm_add_ps(current3X4, tmp2X4);
            simde__m128 limited4X4 = simde_mm_max_ps(current4X4, targetX4);
            simde_mm_storeu_ps(&output[i + 12], limited4X4);
            current = simde_mm_cvtss_f32(simde_mm_shuffle_ps(limited4X4, limited4X4, SIMDE_MM_SHUFFLE(3, 3, 3, 3)));
        }
        else {
            simde_mm_storeu_ps(&output[i], targetX4);
            simde_mm_storeu_ps(&output[i + 4], targetX4);
            simde_mm_storeu_ps(&output[i + 8], targetX4);
            simde_mm_storeu_ps(&output[i + 12], targetX4);
        }
    }
#else
    for (; i + 15 < count; i += 16) {
        const float nextTarget = input[i + 15];

        if (target != nextTarget) {
            target = nextTarget;
            step = (target - current) / std::max(16, smoothFrames);
        }
        if (target > current) {
            for (size_t j = 0; j < 16; ++j)
                output[i + j] = current = std::min(target, current + step);
        }
        else if (target < current) {
            for (size_t j = 0; j < 16; ++j)
                output[i + j] = current = std::max(target, current + step);
        }
        else {
            for (size_t j = 0; j < 16; ++j)
                output[i + j] = target;
        }
    }
#endif

    if (i < count) {
        const float nextTarget = input[count - 1];
        if (target != nextTarget) {
            target = nextTarget;
            step = (target - current) / std::max((int)(count - i), smoothFrames);
        }
        if (target > current) {
            for (; i < count; ++i)
                output[i] = current = std::min(target, current + step);
        }
        else if (target < current) {
            for (; i < count; ++i)
                output[i] = current = std::max(target, current + step);
        }
        else {
            for (; i < count; ++i)
                output[i] = target;
        }
    }

    current_ = current;
    target_ = target;
    step_ = step;
}

void LinearSmoother::updateParameters()
{
    smoothFrames_ = (int)(smoothTime_ * sampleRate_);
}
