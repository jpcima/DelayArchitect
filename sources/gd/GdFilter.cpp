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

#include "GdFilter.h"
#include <cmath>
#include <cstdio>

void GdFilter::updateCoeffs()
{
    Coeff1 c1;
    Coeff2 c2;

    Real fs = sampleRate_;
    Real fc = cutoff_;
    Real q = resonance_;
    Real pi = (Real)M_PI;

    switch (filter_) {
    case kFilterOff:
        // Pass through
        c1.u0 = 1;
        c1.u1 = 0;
        c1.v1 = 0;

        // Pass through
        c2.b0 = 1;
        c2.b1 = 0;
        c2.b2 = 0;
        c2.a1 = 0;
        c2.a2 = 0;
        break;
    case kFilterLPF6:
    {
        // LPF 6dB/oct
        {
            Real c = 1/std::tan(fc*pi/fs);
            c1.u0 = 1/(1+c);
            c1.u1 = c1.u0;
            c1.v1 = (1-c)/(1+c);
        }

        // Peak
        {
        peak:
            Real w = 2*(fc*pi/fs);
            Real A = std::sqrt(q);
            Real S = std::sin(w);
            Real C = std::cos(w);
            Real b0 = 1+S*A;
            Real b1 = -2*C;
            Real b2 = 1-S*A;
            Real a0 = 1+S/A;
            Real a1 = -2*C;
            Real a2 = 1-S/A;
            c2.b0 = b0/a0;
            c2.b1 = b1/a0;
            c2.b2 = b2/a0;
            c2.a1 = a1/a0;
            c2.a2 = a2/a0;
        }
        break;
    }
    case kFilterHPF6:
    {
        // HPF 6dB/oct
        {
            Real c = 1/std::tan(fc*pi/fs);
            c1.u0 = c/(1+c);
            c1.u1 = -c1.u0;
            c1.v1 = (1-c)/(1+c);
        }

        // Peak
        goto peak;
    }
    case kFilterLPF12:
    {
        // Pass through
        c1.u0 = 1;
        c1.u1 = 0;
        c1.v1 = 0;

        // LPF 12dB/oct
        {
            Real w = 2*(fc*pi/fs);
            Real a = std::sin(w)/(2*q);
            Real b0 = (1-std::cos(w))/2;
            Real b1 = 1-std::cos(w);
            Real b2 = (1-std::cos(w))/2;
            Real a0 = 1+a;
            Real a1 = -2*std::cos(w);
            Real a2 = 1-a;
            c2.b0 = b0/a0;
            c2.b1 = b1/a0;
            c2.b2 = b2/a0;
            c2.a1 = a1/a0;
            c2.a2 = a2/a0;
        }
        break;
    }
    case kFilterHPF12:
    {
        // Pass through
        c1.u0 = 1;
        c1.u1 = 0;
        c1.v1 = 0;

        // HPF 12dB/oct
        {
            Real w = 2*(fc*pi/fs);
            Real a = std::sin(w)/(2*q);
            Real b0 = (1+std::cos(w))/2;
            Real b1 = -1-std::cos(w);
            Real b2 = (1+std::cos(w))/2;
            Real a0 = 1+a;
            Real a1 = -2*std::cos(w);
            Real a2 = 1-a;
            c2.b0 = b0/a0;
            c2.b1 = b1/a0;
            c2.b2 = b2/a0;
            c2.a1 = a1/a0;
            c2.a2 = a2/a0;
        }
        break;
    }
    }

    coeff1_ = c1;
    coeff2_ = c2;
}
