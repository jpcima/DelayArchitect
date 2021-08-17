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
    float processOne(float input);

    struct Coeff1 {
        // the first order component
        float u0, u1;
        float v1;
    };
    struct Coeff2 {
        // the second order component
        float b0, b1, b2;
        float a1, a2;
    };

private:
    // the first order filter
    struct Mem1 { float x1, y1; };
    Mem1 mem1_{};
    Coeff1 coeff1_{};

    // the second order filter
    struct Mem2 { float x1, x2, y1, y2; };
    Mem2 mem2_{};
    Coeff2 coeff2_{};

    // controls
    int filter_ = GdFilterOff;
    float sampleRate_ = 0;
    float cutoff_ = 0;
    float resonance_ = 0;
};

#include "GdFilter.hpp"
