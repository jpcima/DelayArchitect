// SPDX-License-Identifier: BSD-2-Clause

// This code is part of the sfizz library and is licensed under a BSD 2-clause
// license. You should have receive a LICENSE.md file along with the code.
// If not, contact the sfizz maintainers at https://github.com/sfztools/sfizz

#pragma once

/**
 * @brief Linear smoother
 *
 */
class LinearSmoother {
public:
    /**
     * @brief Set the sample rate.
     *
     * @param sampleRate
     */
    void setSampleRate(float sampleRate);
    /**
     * @brief Set the smoothing time in seconds.
     *
     * @param smoothValue
     * @param sampleRate
     */
    void setSmoothTime(float smoothTime);
    /**
     * @brief Reset the filter state to a given value
     *
     * @param value
     */
    void clear(float value = 0.0f);
    /**
     * @brief Reset to the target value (the back of the last vector passed)
     */
    void clearToTarget();
    /**
     * @brief Process a span of data. Input and output can refer to the same
     * memory.
     *
     * @param input
     * @param output
     * @param count
     * @param canShortcut whether we can have a fast path if the filter is within
     *                    a reasonable range around the first value of the input
     *                    span.
     */
    void process(const float *input, float *output, unsigned count, bool canShortcut = false);
    /**
     * @brief Get the current value
     */
    float current() const { return current_; }

private:
    void updateParameters();

private:
    float current_ = 0.0;
    float target_ = 0.0;
    float step_ = 0.0;
    int smoothFrames_ = 0;
    float smoothTime_ = 0;
    float sampleRate_ = 0;
};
