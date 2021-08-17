#include "GdFilter.h"

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

inline GdFilter::Real GdFilter::processOne(Real input)
{
    Real output;

    // First order part
    {
        const Coeff1 c = coeff1_;
        Mem1 m = mem1_;
        output = c.u0 * input + c.u1 * m.x1 - c.v1 * m.y1;
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
        m.s1 = m.s2 + c.b1 * input - c.a1 * output;
        m.s2 = c.b2 * input - c.a2 * output;
        mem2_ = m;
    }

    return output;
}
