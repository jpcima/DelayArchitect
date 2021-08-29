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
#include <SoundTouch.h>

#define GD_SHIFTER_UPDATES_AT_K_RATE 1
#define GD_SHIFTER_CAN_REPORT_LATENCY 1

class GdShifter {
public:
    GdShifter();
    ~GdShifter();

    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize) { (void)bufferSize; }
    void setShift(float shiftLinear);
    float processOne(float input);
    void process(const float *input, float *output, unsigned count);
    float getLatency() const;

private:
    soundtouch::SoundTouch st_;
    float shift_ = 1.0f;
    float sampleRate_ = 0.0f;
};
