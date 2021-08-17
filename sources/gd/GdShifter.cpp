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

void GdShifter::process(const float *input, float *output, const float *shiftLinear, unsigned count)
{
    for (unsigned i = 0; i < count; ++i)
        output[i] = processOne(input[i], shiftLinear[i]);
}
