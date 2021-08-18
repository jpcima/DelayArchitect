#include "GdShifter.h"
#include "utility/NextPowerOfTwo.h"
#include <cmath>
#include <cstring>

GdShifter::GdShifter()
{
    soundtouch::SoundTouch &st = st_;
    st.setChannels(1);
    setSampleRate(44100);
}

GdShifter::~GdShifter()
{
}

void GdShifter::clear()
{
    soundtouch::SoundTouch &st = st_;
    st.clear();
}

void GdShifter::setSampleRate(float sampleRate)
{
    if (sampleRate_ == sampleRate)
        return;

    sampleRate_ = sampleRate;
    soundtouch::SoundTouch &st = st_;
    st.setSampleRate(sampleRate);
    clear();
}

void GdShifter::setShift(float shiftLinear)
{
    soundtouch::SoundTouch &st = st_;
    float oldShift = shift_;

    if (oldShift == shiftLinear)
        return;

    shift_ = shiftLinear;
    if (shiftLinear != 1.0f) {
        st.setPitch(shiftLinear);
        if (oldShift == 1.0f)
            clear();
    }
}

float GdShifter::processOne(float input)
{
    if (shift_ == 1.0f)
        return input;

    soundtouch::SoundTouch &st = st_;
    st.putSamples(&input, 1);
    float output = 0;
    st.receiveSamples(&output, 1);

    return output;
}

void GdShifter::process(const float *input, float *output, unsigned count)
{
    if (shift_ == 1.0f) {
        if (input != output)
            std::memcpy(output, input, count * sizeof(float));
    }
    else {
        soundtouch::SoundTouch &st = st_;

        unsigned index = 0;
        while (index < count) {
            unsigned blockSize = count - index;
            blockSize = (blockSize < 32) ? blockSize : 32;

            st.putSamples(input + index, count - index);

            float block[32];
            unsigned received = st.receiveSamples(block, blockSize);

            float *dst = output + index;
            std::memset(dst, 0, (blockSize - received) * sizeof(float));
            std::memcpy(dst + (blockSize - received), block, received * sizeof(float));

            index += blockSize;
        }
    }
}

float GdShifter::getLatency() const
{
    const soundtouch::SoundTouch &st = st_;
    int initialLatency = st.getSetting(SETTING_INITIAL_LATENCY);
    int outputSequence = st.getSetting(SETTING_NOMINAL_OUTPUT_SEQUENCE);

    int latencySamples = initialLatency - outputSequence / 2;
    return latencySamples / sampleRate_;
}
