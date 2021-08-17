#pragma once
#include <SoundTouch.h>
#include <vector>

#if GD_USE_SOUNDTOUCH_SHIFTER

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

#else

class GdShifter {
public:
    void clear();
    void setSampleRate(float sampleRate);
    float processOne(float input, float shiftLinear);
    void process(const float *input, float *output, const float *shiftLinear, unsigned count);
    float getLatency() const { return 0; } // not implemented

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

#endif

#include "GdShifter.hpp"
