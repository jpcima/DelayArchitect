#pragma once
#include <SoundTouch.h>

#define GD_SHIFTER_UPDATES_AT_K_RATE 1
#define GD_SHIFTER_CAN_REPORT_LATENCY 1

class GdShifter {
public:
    GdShifter();
    ~GdShifter();

    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize) { (void)bufferSize; }
    void setShift(float shiftLinear);
    float processOne(float input);
    void process(const float *input, float *output, unsigned count);
    float getLatency() const;

private:
    soundtouch::SoundTouch st_;
    float shift_ = 1.0f;
    float sampleRate_ = 0.0f;
};
