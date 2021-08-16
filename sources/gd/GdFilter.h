#pragma once
#include "GdDefs.h"

class GdFilter {
public:
    enum FilterType {
        kFilterOff,
        kFilterLPF6,
        kFilterHPF6,
        kFilterLPF12,
        kFilterHPF12,
    };

    void clear();
    void setSampleRate(float sampleRate);
    void setFilterType(int filter);
    void setCutoff(float cutoff);
    void setResonance(float resonance);
    void updateCoeffs();

    struct Coeff {
        // the first order component
        float u0, u1;
        float v1;
        // the second order component
        float b0, b1, b2;
        float a1, a2;
    };

private:
    // the first order memories
    float t1_ = 0;

    // the second order memories
    float s1_ = 0;
    float s2_ = 0;

    // coefficients
    Coeff coeff_ {};

    // controls
    int filter_ = GdFilterOff;
    float sampleRate_ = 0;
    float cutoff_ = 0;
    float resonance_ = 0;
};

#include "GdFilter.hpp"
