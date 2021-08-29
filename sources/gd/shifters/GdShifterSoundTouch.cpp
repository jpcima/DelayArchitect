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

#include "GdShifter.h"
#include "utility/NextPowerOfTwo.h"
#include <cmath>
#include <cstring>

GdShifter::GdShifter()
{
    soundtouch::SoundTouch &st = st_;
    st.setChannels(1);
    setSampleRate(44100);
}

GdShifter::~GdShifter()
{
}

void GdShifter::clear()
{
    soundtouch::SoundTouch &st = st_;
    st.clear();
}

void GdShifter::setSampleRate(float sampleRate)
{
    if (sampleRate_ == sampleRate)
        return;

    sampleRate_ = sampleRate;
    soundtouch::SoundTouch &st = st_;
    st.setSampleRate(sampleRate);
    clear();
}

void GdShifter::setShift(float shiftLinear)
{
    soundtouch::SoundTouch &st = st_;
    float oldShift = shift_;

    if (oldShift == shiftLinear)
        return;

    shift_ = shiftLinear;
    if (shiftLinear != 1.0f) {
        st.setPitch(shiftLinear);
        if (oldShift == 1.0f)
            clear();
    }
}

float GdShifter::processOne(float input)
{
    if (shift_ == 1.0f)
        return input;

    soundtouch::SoundTouch &st = st_;
    st.putSamples(&input, 1);
    float output = 0;
    st.receiveSamples(&output, 1);

    return output;
}

void GdShifter::process(const float *input, float *output, unsigned count)
{
    if (shift_ == 1.0f) {
        if (input != output)
            std::memcpy(output, input, count * sizeof(float));
    }
    else {
        soundtouch::SoundTouch &st = st_;

        unsigned index = 0;
        while (index < count) {
            unsigned blockSize = count - index;
            blockSize = (blockSize < 32) ? blockSize : 32;

            st.putSamples(input + index, count - index);

            float block[32];
            unsigned received = st.receiveSamples(block, blockSize);

            float *dst = output + index;
            std::memset(dst, 0, (blockSize - received) * sizeof(float));
            std::memcpy(dst + (blockSize - received), block, received * sizeof(float));

            index += blockSize;
        }
    }
}

float GdShifter::getLatency() const
{
    const soundtouch::SoundTouch &st = st_;
    int initialLatency = st.getSetting(SETTING_INITIAL_LATENCY);
    int outputSequence = st.getSetting(SETTING_NOMINAL_OUTPUT_SEQUENCE);

    int latencySamples = initialLatency - outputSequence / 2;
    return latencySamples / sampleRate_;
}
