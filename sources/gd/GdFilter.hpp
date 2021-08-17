#include "GdFilter.h"

inline void GdFilter::clear()
{
    mem1_ = Mem1{};
    mem2_ = Mem2{};
}

inline void GdFilter::setSampleRate(float sampleRate)
{
    sampleRate_ = sampleRate;
}

inline void GdFilter::setFilterType(int filter)
{
    filter_ = filter;
}

inline void GdFilter::setCutoff(float cutoff)
{
    cutoff_ = cutoff;
}

inline void GdFilter::setResonance(float resonance)
{
    resonance_ = resonance;
}

inline float GdFilter::processOne(float input)
{
    float output;

    // First order part
    {
        const Coeff1 c = coeff1_;
        Mem1 m = mem1_;
        output = c.u0 * input + c.u1 * m.x1 + c.v1 * m.y1;
        m.x1 = input;
        m.y1 = output;
        mem1_ = m;
    }

    input = output;

    // Second order part
    {
        const Coeff2 c = coeff2_;
        Mem2 m = mem2_;
        output = c.b0 * input + c.b1 * m.x1 + c.b2 * m.x2 + c.a1 * m.y1 + c.a2 * m.y2;
        m.x2 = m.x1;
        m.x1 = input;
        m.y2 = m.y1;
        m.y1 = output;
        mem2_ = m;
    }

    return output;
}
