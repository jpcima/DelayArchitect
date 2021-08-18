#pragma once
#include <vector>

#define GD_SHIFTER_UPDATES_AT_K_RATE 0
#define GD_SHIFTER_CAN_REPORT_LATENCY 0

class GdShifter {
public:
    void clear();
    void setSampleRate(float sampleRate);
    void setBufferSize(unsigned bufferSize) { (void)bufferSize; }
    float processOne(float input, float shiftLinear);
    void process(const float *input, float *output, const float *shiftLinear, unsigned count);

private:
    // fixed characteristics
    static constexpr float getWindowTime() { return 60e-3f; }
    static constexpr float getCrossfadeRatio() { return 0.05f; }

private:
    float d_ = 0;
    float w_ = 0;
    float fs_ = 0;
    unsigned li_ = 0;
    std::vector<float> l_;
};

#include "GdShifterSimple.hpp"
