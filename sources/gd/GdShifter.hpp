#include "GdShifter.h"
#include <cmath>

inline void GdShifter::clear()
{
    std::fill(l_.begin(), l_.end(), 0.0f);
    d_ = 0;
    li_ = 0;
}

inline float GdShifter::processOne(float input, float shiftLinear)
{
    float output;
    float *l = l_.data();
    unsigned ln = (unsigned)l_.size();
    unsigned li = li_;
    float d = d_;
    float w = w_;
    constexpr float cr = getCrossfadeRatio();

    //float shiftLinear = std::exp2(shift * (1.0f / 12));
    float i = 1 - shiftLinear;
    d = d + i + w;
    d = d - w * (int)(d / w); //d = std::fmod(d, w);

    l[li] = input;

    auto fdelay = [l, li, ln](float d) -> float {
        float p = li + d;
        int p1 = (int)p;
        int p2 = p1 + 1;
        float f = p - (int)p;
        float y1 = l[(unsigned)p1 & (ln - 1)];
        float y2 = l[(unsigned)p2 & (ln - 1)];
        return y1 + f * (y2 - y1);
    };

    float c = d / (cr * w);
    output = fdelay(d) * c + fdelay(d + w) * (1.0f - c);

    // in case of no shifting, return input directly (no latency)
    output = (shiftLinear != 1.0f) ? output : input;

    d_ = d;
    li_ = (li + ln - 1) & (ln - 1);
    return output;
}
