#include "GdShifter.h"
#include "utility/NextPowerOfTwo.h"
#include <cmath>
#include <cstdio>

void GdShifter::setSampleRate(float fs)
{
    if (fs_ == fs)
        return;

    constexpr float tw = getWindowTime();
    unsigned w = std::ceil(tw * fs);

    l_.resize(nextPowerOfTwo(2 * w));
    w_ = (float)w;
    fs_ = fs;

    clear();
}
