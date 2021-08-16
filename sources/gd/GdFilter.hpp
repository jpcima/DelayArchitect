#include "GdFilter.h"

inline void GdFilter::clear()
{
    t1_ = 0;
    s1_ = 0;
    s2_ = 0;
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
