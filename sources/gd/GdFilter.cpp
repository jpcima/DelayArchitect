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
