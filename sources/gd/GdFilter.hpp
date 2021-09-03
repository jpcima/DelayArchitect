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
#include "utility/RsqrtNL.h"
#include <cmath>

inline GdFilter::Real GdFilter::Linearity::operator()(Real x) const
{
    return x;
}

inline GdFilter::Real GdFilter::SaturatingNonLinearity::operator()(Real x) const
{
    return rsqrtNL(x);
}

inline void GdFilter::clear()
{
    mem1_ = Mem1{};
    mem2_ = Mem2{};
}

inline void GdFilter::setSampleRate(Real sampleRate)
{
    sampleRate_ = sampleRate;
}

inline int GdFilter::getFilterType() const
{
    return filter_;
}

inline void GdFilter::setFilterType(int filter)
{
    filter_ = filter;
}

inline GdFilter::Real GdFilter::getCutoff() const
{
    return cutoff_;
}

inline void GdFilter::setCutoff(Real cutoff)
{
    cutoff_ = cutoff;
}

inline GdFilter::Real GdFilter::getResonance() const
{
    return resonance_;
}

inline void GdFilter::setResonance(Real resonance)
{
    resonance_ = resonance;
}

inline bool GdFilter::isAnalog() const
{
    return processOneFunction_ == &GdFilter::processOneNL<SaturatingNonLinearity>;
}

inline void GdFilter::setAnalog(bool analog)
{
    Real (GdFilter:: *function)(Real) = analog ?
        &GdFilter::processOneNL<SaturatingNonLinearity> :
        &GdFilter::processOneNL<Linearity>;

    if (processOneFunction_ == function)
        return;

    processOneFunction_ = function;
    clear();
}

inline GdFilter::Real GdFilter::processOne(Real input)
{
    return (this->*processOneFunction_)(input);
}

template <class T> inline void GdFilter::process(const T *input, T *output, unsigned count)
{
    if (isAnalog())
        processNL<T, SaturatingNonLinearity>(input, output, count);
    else
        processNL<T, Linearity>(input, output, count);
}

template <class NL> inline GdFilter::Real GdFilter::processOneNL(Real input)
{
    Real output;
    NL nl;

    // First order part
    {
        const Coeff1 c = coeff1_;
        Mem1 m = mem1_;
        output = c.u0 * input + nl(c.u1 * m.x1 - c.v1 * m.y1);
        m.x1 = input;
        m.y1 = output;
        mem1_ = m;
    }

    input = output;

    // Second order part
    {
        const Coeff2 c = coeff2_;
        Mem2 m = mem2_;
        output = m.s1 + c.b0 * input;
        m.s1 = nl(m.s2 + c.b1 * input - c.a1 * output);
        m.s2 = nl(c.b2 * input - c.a2 * output);
        mem2_ = m;
    }

    return output;
}

template <class T, class NL> inline void GdFilter::processNL(const T *input, T *output, unsigned count)
{
    GdFilter filter = *this;

    for (unsigned i = 0; i < count; ++i)
        output[i] = filter.processOneNL<NL>((Real)input[i]);

    mem1_ = filter.mem1_;
    mem2_ = filter.mem2_;
}
