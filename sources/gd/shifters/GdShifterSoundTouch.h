#pragma once
#include <SoundTouch.h>

class GdShifter {
public:
    GdShifter();
    ~GdShifter();

    void clear();
    void setSampleRate(float sampleRate);
    void setShift(float shiftLinear);
    float processOne(float input);
    void process(const float *input, float *output, unsigned count);
    float getLatency() const;

private:
    soundtouch::SoundTouch st_;
    float shift_ = 1.0f;
    float sampleRate_ = 0.0f;
};
