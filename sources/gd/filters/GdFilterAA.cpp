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

#include "GdFilterAA.h"
#include <array>

const float *GdFilterAA::neutralCoeffs_ = []() -> const float * {
    static std::array<float, 5 * NS + 1> coeffs;
    float *cptr = coeffs.data();
    for (unsigned nthSection = 0; nthSection < NS; ++nthSection) {
        *cptr++ = 1.0f; *cptr++ = 0.0f; *cptr++ = 0.0f;
        *cptr++ = 0.0f; *cptr++ = 0.0f;
    }
    *cptr++ = 1.0f;
    return coeffs.data();
}();


void GdFilterAA::setSampleRate(float newSampleRate)
{
    if (sampleRate_ == newSampleRate)
        return;

    sampleRate_ = newSampleRate;
    updateCoeffs();
}

void GdFilterAA::setCutoff(float newCutoff)
{
    if (cutoff_ == newCutoff)
        return;

    cutoff_ = newCutoff;
    updateCoeffs();
}

void GdFilterAA::updateCoeffs()
{
    float F = cutoff_ / sampleRate_;
    if (F >= 0.5f) {
        coeffs_ = neutralCoeffs_;
    }
    else {
        int index = (int)(0.5f + (NF - 1) * ((F - F0) / (F1 - F0)));
        index = (index < 0) ? 0 : index;
        index = (index > NF - 1) ? (NF - 1) : index;
        coeffs_ = GdFilterDataAA::BA + (unsigned)index * (5 * NS + 1);
    }
}

void GdFilterAA::process(const float *input, float *output, unsigned count)
{
    // compute sections except last
    for (unsigned nthSection = 0; nthSection < NS - 1; ++nthSection) {
        Section sec = sec_[nthSection];
        const float *sc = coeffs_ + 5 * nthSection;

        float b0 = sc[0], b1 = sc[1], b2 = sc[2];
        float a1 = sc[3], a2 = sc[4];

        for (unsigned i = 0; i < count; ++i) {
            float in = input[i];
            float out = sec.s1 + b0 * in;
            sec.s1 = sec.s2 + b1 * in - a1 * out;
            sec.s2 = b2 * in - a2 * out;
            output[i] = out;
        }

        sec_[nthSection] = sec;
        input = output;
    }

    // compute final section with gain
    {
        unsigned nthSection = NS - 1;
        Section sec = sec_[nthSection];
        const float *sc = coeffs_ + 5 * nthSection;

        float b0 = sc[0], b1 = sc[1], b2 = sc[2];
        float a1 = sc[3], a2 = sc[4], k = sc[5];

        for (unsigned i = 0; i < count; ++i) {
            float in = input[i];
            float out = sec.s1 + b0 * in;
            sec.s1 = sec.s2 + b1 * in - a1 * out;
            sec.s2 = b2 * in - a2 * out;
            output[i] = k * out;
        }

        sec_[nthSection] = sec;
    }
}

constexpr float GdFilterAA::F0;
constexpr float GdFilterAA::F1;
constexpr unsigned int GdFilterAA::NF;
constexpr unsigned int GdFilterAA::NS;
