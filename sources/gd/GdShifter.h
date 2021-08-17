#pragma once
#include <vector>

class GdShifter {
public:
    void clear();
    void setSampleRate(float sampleRate);
    float processOne(float input, float shiftLinear);

private:
    // fixed characteristics
    static constexpr float getWindowTime() { return 60e-3f; }
    static constexpr float getCrossfadeRatio() { return 1.0f; }

private:
    float d_ = 0;
    float w_ = 0;
    float fs_ = 0;
    unsigned li_ = 0;
    std::vector<float> l_;
};

#include "GdShifter.hpp"
