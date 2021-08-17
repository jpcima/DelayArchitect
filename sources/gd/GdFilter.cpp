#include "GdFilter.h"
#include <cmath>

void GdFilter::updateCoeffs()
{
    Coeff1 c1;
    Coeff2 c2;

    float fs = sampleRate_;
    float fc = cutoff_;
    float q = resonance_;
    float pi = (float)M_PI;

    switch (filter_) {
    case kFilterOff:
        // Pass through
        c1.u0 = 1.0f;
        c1.u1 = 0.0f;
        c1.v1 = 0.0f;

        // Pass through
        c2.b0 = 1.0f;
        c2.b1 = 0.0f;
        c2.b2 = 0.0f;
        c2.a1 = 0.0f;
        c2.a2 = 0.0f;
        break;
    case kFilterLPF6:
    {
        // LPF 6dB/oct
        {
            float c = 1/std::tan(fc*pi/fs);
            c1.u0 = 1/(1+c);
            c1.u1 = c1.u0;
            c1.v1 = (1-c)/(1+c);
        }

        // Peak
        {
        peak:
            float w = 2*(fc*pi/fs);
            float A = std::sqrt(q);
            float S = std::sin(w);
            float C = std::cos(w);
            float b0 = 1+S*A;
            float b1 = -2*C;
            float b2 = 1-S*A;
            float a0 = 1+S/A;
            float a1 = -2*C;
            float a2 = 1-S/A;
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
            float c = 1/std::tan(fc*pi/fs);
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
        c1.u0 = 1.0f;
        c1.u1 = 0.0f;
        c1.v1 = 0.0f;

        // LPF 12dB/oct
        {
            float w = 2*(fc*pi/fs);
            float a = std::sin(w)/(2*q);
            float b0 = 0.5f*(1-std::cos(w));
            float b1 = 1-std::cos(w);
            float b2 = 0.5f*(1-std::cos(w));
            float a0 = 1+a;
            float a1 = -2*std::cos(w);
            float a2 = 1-a;
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
        c1.u0 = 1.0f;
        c1.u1 = 0.0f;
        c1.v1 = 0.0f;

        // HPF 12dB/oct
        {
            float w = 2*(fc*pi/fs);
            float a = std::sin(w)/(2*q);
            float b0 = 0.5f*(1+std::cos(w));
            float b1 = -1-std::cos(w);
            float b2 = 0.5f*(1+std::cos(w));
            float a0 = 1+a;
            float a1 = -2*std::cos(w);
            float a2 = 1-a;
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
