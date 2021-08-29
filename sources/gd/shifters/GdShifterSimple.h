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

#define GD_SHIFTER_UPDATES_AT_K_RATE 0
#define GD_SHIFTER_CAN_REPORT_LATENCY 0

class GdShifter {
public:
    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize) { (void)bufferSize; }
    float processOne(float input, float shiftLinear);
    void process(const float *input, float *output, const float *shiftLinear, unsigned count);

private:
    // fixed characteristics
    static constexpr float getWindowTime() { return 60e-3f; }
    static constexpr float getCrossfadeRatio() { return 0.05f; }

private:
    float d_ = 0;
    float w_ = 0;
    float fs_ = 0;
    unsigned li_ = 0;
    std::vector<float> l_;
};

#include "GdShifterSimple.hpp"
