#include "GdFilter.h"
#include <cmath>

void GdFilter::updateCoeffs()
{
    Coeff co;

    float fs = sampleRate_;
    float fc = cutoff_;
    float q = resonance_;
    float pi = (float)M_PI;

    switch (filter_) {
    case kFilterOff:
        // Pass through
        co.u0 = 1.0f;
        co.u1 = 0.0f;
        co.v1 = 0.0f;

        // Pass through
        co.b0 = 1.0f;
        co.b1 = 0.0f;
        co.b2 = 0.0f;
        co.a1 = 0.0f;
        co.a2 = 0.0f;
        break;
    case kFilterLPF6:
    {
        // LPF 6dB/oct
        {
            float c = 1/std::tan(fc*pi/fs);
            co.u0 = 1/(1+c);
            co.u1 = co.u0;
            co.v1 = (1-c)/(1+c);
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
            co.b0 = b0/a0;
            co.b1 = b1/a0;
            co.b2 = b2/a0;
            co.a1 = a1/a0;
            co.a2 = a2/a0;
        }
        break;
    }
    case kFilterHPF6:
    {
        // HPF 6dB/oct
        {
            float c = 1/std::tan(fc*pi/fs);
            co.u0 = c/(1+c);
            co.u1 = -co.u0;
            co.v1 = (1-c)/(1+c);
        }

        // Peak
        goto peak;
    }
    case kFilterLPF12:
    {
        // Pass through
        co.u0 = 1.0f;
        co.u1 = 0.0f;
        co.v1 = 0.0f;

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
            co.b0 = b0/a0;
            co.b1 = b1/a0;
            co.b2 = b2/a0;
            co.a1 = a1/a0;
            co.a2 = a2/a0;
        }
        break;
    }
    case kFilterHPF12:
    {
        // Pass through
        co.u0 = 1.0f;
        co.u1 = 0.0f;
        co.v1 = 0.0f;

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
            co.b0 = b0/a0;
            co.b1 = b1/a0;
            co.b2 = b2/a0;
            co.a1 = a1/a0;
            co.a2 = a2/a0;
        }
        break;
    }
    }

    coeff_ = co;
}
