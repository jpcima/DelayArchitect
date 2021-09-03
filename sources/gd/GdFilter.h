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

class GdFilter {
public:
    using Real = double;

    enum FilterType {
        kFilterOff,
        kFilterLPF6,
        kFilterHPF6,
        kFilterLPF12,
        kFilterHPF12,
    };

    void clear();
    void setSampleRate(Real sampleRate);
    int getFilterType() const;
    void setFilterType(int filter);
    Real getCutoff() const;
    void setCutoff(Real cutoff);
    Real getResonance() const;
    void setResonance(Real resonance);
    bool isAnalog() const;
    void setAnalog(bool analog);
    void updateCoeffs();
    template <class T> void process(const T *input, T *output, unsigned count);
    Real processOne(Real input);

    struct Linearity {
        Real operator()(Real x) const;
    };
    struct SaturatingNonLinearity {
        Real operator()(Real x) const;
    };

    template <class T, class NL> void processNL(const T *input, T *output, unsigned count);
    template <class NL> Real processOneNL(Real input);

    struct Coeff1 {
        // the first order component
        Real u0, u1;
        Real v1;
    };
    struct Coeff2 {
        // the second order component
        Real b0, b1, b2;
        Real a1, a2;
    };

private:
    // the first order filter
    struct Mem1 { Real x1, y1; };
    Mem1 mem1_{};
    Coeff1 coeff1_{};

    // the second order filter (transposed form II)
    struct Mem2 { Real s1, s2; };
    Mem2 mem2_{};
    Coeff2 coeff2_{};

    // controls
    int filter_ = kFilterOff;
    Real sampleRate_ = 0;
    Real cutoff_ = 0;
    Real resonance_ = 0;

    // digital/analog
    Real (GdFilter:: *processOneFunction_)(Real) = &GdFilter::processOneNL<Linearity>;
};

#include "GdFilter.hpp"
