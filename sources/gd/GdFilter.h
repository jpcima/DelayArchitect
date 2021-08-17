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
    void updateCoeffs();
    Real processOne(Real input);

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
};

#include "GdFilter.hpp"
