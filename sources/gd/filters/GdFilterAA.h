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
#include "GdFilterDataAA.hxx"

class GdFilterAA {
public:
    void clear();
    float getSampleRate() const;
    void setSampleRate(float newSampleRate);
    void setCutoff(float newCutoff);
    void updateCoeffs();
    void process(const float *input, float *output, unsigned count);
    float processOne(float input);

private:
    static constexpr float F0 = GdFilterDataAA::F0;
    static constexpr float F1 = GdFilterDataAA::F1;
    static constexpr unsigned int NF = GdFilterDataAA::NF;
    static constexpr unsigned int NS = GdFilterDataAA::NS;

    struct Section { float s1, s2; };
    Section sec_[NS] {};

    float sampleRate_ = 0;
    float cutoff_ = 0;
    const float *coeffs_ = GdFilterDataAA::BA;
};

#include "GdFilterAA.hpp"
