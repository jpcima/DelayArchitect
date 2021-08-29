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
#include "utility/NextPowerOfTwo.h"
#include <cmath>

void GdShifter::setSampleRate(float fs)
{
    if (fs_ == fs)
        return;

    constexpr float tw = getWindowTime();
    unsigned w = std::ceil(tw * fs);

    l_.resize(nextPowerOfTwo(2 * w));
    w_ = (float)w;
    fs_ = fs;

    clear();
}

void GdShifter::process(const float *input, float *output, const float *shiftLinear, unsigned count)
{
    for (unsigned i = 0; i < count; ++i)
        output[i] = processOne(input[i], shiftLinear[i]);
}
