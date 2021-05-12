#pragma once
#include <cmath>

inline float db2linear(float db)
{
    return std::pow(10.0f, 0.05f * db);
}

inline float linear2db(float linear)
{
    return 20.0f * std::log10(linear);
}
