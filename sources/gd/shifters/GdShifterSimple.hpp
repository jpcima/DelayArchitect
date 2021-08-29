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

#include "GdShifterSimple.h"
#include <cmath>

inline void GdShifter::clear()
{
    std::fill(l_.begin(), l_.end(), 0.0f);
    d_ = 0;
    li_ = 0;
}

inline float GdShifter::processOne(float input, float shiftLinear)
{
    float output;
    float *l = l_.data();
    unsigned ln = (unsigned)l_.size();
    unsigned li = li_;
    float d = d_;
    float w = w_;
    constexpr float cr = getCrossfadeRatio();

    //float shiftLinear = std::exp2(shift * (1.0f / 12));
    float i = 1 - shiftLinear;
    d = d + i + w;
    d = d - w * (int)(d / w); //d = std::fmod(d, w);

    l[li] = input;

    auto fdelay = [l, li, ln](float d) -> float {
        float p = li + d;
        int p1 = (int)p;
        int p2 = p1 + 1;
        float f = p - (int)p;
        float y1 = l[(unsigned)p1 & (ln - 1)];
        float y2 = l[(unsigned)p2 & (ln - 1)];
        return y1 + f * (y2 - y1);
    };

    float c = std::fmin(1.0f, d / (cr * w));
    output = fdelay(d) * c + fdelay(d + w) * (1.0f - c);

    // in case of no shifting, return input directly (no latency)
    output = (shiftLinear != 1.0f) ? output : input;

    d_ = d;
    li_ = (li + ln - 1) & (ln - 1);
    return output;
}
