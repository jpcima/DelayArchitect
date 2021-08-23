// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#ifndef Ignorable
#   if defined(__GNUC__)
#       define Ignorable __attribute__((unused))
#   else
#       define Ignorable
#   endif
#endif

enum {
    // maximum number of delay lines
    GdMaxLines = 26,
    // maximum delay length in seconds
    GdMaxDelay = 10,
};

enum GdFilterType {
    GdFilterOff = -1,
    //
    GdFilter6dB,
    GdFilter12dB,
    //
    GdNumFilterTypes,
};

// parameter smoothing time in seconds
Ignorable static constexpr float GdParamSmoothTime = 50e-3f;

// smallest gain setting
Ignorable static constexpr float GdMinFeedbackGainLinear = 1e-3f;
Ignorable static constexpr float GdMinFeedbackGainDB = -60.0f;

#define GD_EACH_PARAMETER(_)                                                   \
    /* Name, Min, Max, Def, Flags, Label, Group */                             \
    _(SYNC, 0, 1, 1, GDP_BOOLEAN, "Synchronization", -1)                       \
    _(GRID, GdMinDivisor, GdMaxDivisor, GdDefaultDivisor, GDP_INTEGER, "Grid", -1) \
    _(SWING, 0, 1, 0.5, GDP_FLOAT, "Swing", -1)                                \
    _(FEEDBACK_ENABLE, false, true, false, GDP_BOOLEAN, "Feedback Enable", -1) \
    _(FEEDBACK_TAP, 0, GdMaxLines - 1, 0, GDP_CHOICE, "Feedback Tap", -1)      \
    _(FEEDBACK_GAIN, GdMinFeedbackGainDB, 0, GdMinFeedbackGainDB, GDP_FLOAT, "Feedback Gain", -1) \
    _(MIX_DRY, -60, 10, -6, GDP_FLOAT, "Dry mix", -1)                          \
    _(MIX_WET, -60, 10, -6, GDP_FLOAT, "Wet mix", -1)                          \
    GD_EACH_LINE_PARAMETER(_, A, 0)                                            \
    GD_EACH_LINE_PARAMETER(_, B, 1)                                            \
    GD_EACH_LINE_PARAMETER(_, C, 2)                                            \
    GD_EACH_LINE_PARAMETER(_, D, 3)                                            \
    GD_EACH_LINE_PARAMETER(_, E, 4)                                            \
    GD_EACH_LINE_PARAMETER(_, F, 5)                                            \
    GD_EACH_LINE_PARAMETER(_, G, 6)                                            \
    GD_EACH_LINE_PARAMETER(_, H, 7)                                            \
    GD_EACH_LINE_PARAMETER(_, I, 8)                                            \
    GD_EACH_LINE_PARAMETER(_, J, 9)                                            \
    GD_EACH_LINE_PARAMETER(_, K, 10)                                           \
    GD_EACH_LINE_PARAMETER(_, L, 11)                                           \
    GD_EACH_LINE_PARAMETER(_, M, 12)                                           \
    GD_EACH_LINE_PARAMETER(_, N, 13)                                           \
    GD_EACH_LINE_PARAMETER(_, O, 14)                                           \
    GD_EACH_LINE_PARAMETER(_, P, 15)                                           \
    GD_EACH_LINE_PARAMETER(_, Q, 16)                                           \
    GD_EACH_LINE_PARAMETER(_, R, 17)                                           \
    GD_EACH_LINE_PARAMETER(_, S, 18)                                           \
    GD_EACH_LINE_PARAMETER(_, T, 19)                                           \
    GD_EACH_LINE_PARAMETER(_, U, 10)                                           \
    GD_EACH_LINE_PARAMETER(_, V, 21)                                           \
    GD_EACH_LINE_PARAMETER(_, W, 22)                                           \
    GD_EACH_LINE_PARAMETER(_, X, 23)                                           \
    GD_EACH_LINE_PARAMETER(_, Y, 24)                                           \
    GD_EACH_LINE_PARAMETER(_, Z, 25)                                           \
    /* End */

#define GD_EACH_LINE_PARAMETER(_, X, I)                                        \
    /* Name, Min, Max, Def, Flags, Label */                                    \
    /* NOTE: Tap Enable must always appear first */                            \
    _(TAP_##X##_ENABLE, false, true, false, GDP_BOOLEAN, "Tap " #X " Enable", I) \
    _(TAP_##X##_DELAY, 0, GdMaxDelay, 0, GDP_FLOAT, "Tap " #X " Delay", I)     \
    _(TAP_##X##_LEVEL, -20, 10, 0, GDP_FLOAT, "Tap " #X " Level", I)           \
    _(TAP_##X##_FILTER_ENABLE, false, true, false, GDP_BOOLEAN, "Tap " #X " Filter Enable", I) \
    _(TAP_##X##_FILTER, 0, (GdNumFilterTypes - 1), 0, GDP_CHOICE, "Tap " #X " Filter", I) \
    _(TAP_##X##_LPF_CUTOFF, 10, 22000, 22000, GDP_FLOAT, "Tap " #X " LPF Cutoff", I) \
    _(TAP_##X##_HPF_CUTOFF, 10, 22000, 0, GDP_FLOAT, "Tap " #X " HPF Cutoff", I) \
    _(TAP_##X##_RESONANCE, 0, 24, 0, GDP_FLOAT, "Tap " #X " Resonance", I)     \
    _(TAP_##X##_TUNE_ENABLE, false, true, false, GDP_BOOLEAN, "Tap " #X " Tune Enable", I) \
    _(TAP_##X##_TUNE, -1200, 1200, 0, GDP_FLOAT, "Tap " #X " Tune", I)         \
    _(TAP_##X##_PAN, -1, 1, 0, GDP_FLOAT, "Tap " #X " Pan", I)                 \
    _(TAP_##X##_WIDTH, 0, 10, 1, GDP_FLOAT, "Tap " #X " Width", I)             \
    _(TAP_##X##_FLIP, false, true, false, GDP_BOOLEAN, "Tap " #X " Flip", I)   \
    /* End */

typedef enum GdParameter {
    GDP_NONE = -1,
    ///
    #define EACH(p, min, max, def, flags, label, group) GDP_##p,
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    ///
    GD_PARAMETER_COUNT,
} GdParameter;

typedef enum GdParameterFlag {
    GDP_FLOAT       = 0,
    GDP_BOOLEAN     = 1,
    GDP_INTEGER     = 2,
    GDP_CHOICE      = 4,
    GDP_LOGARITHMIC = 8
} GdParameterFlag;

enum {
    GdFirstParameterOfFirstTap = GDP_TAP_A_ENABLE,
    GdNumPametersPerTap = GDP_TAP_B_ENABLE - GDP_TAP_A_ENABLE,
};

inline GdParameter GdDecomposeParameter(GdParameter parameter, int *de_tap_ptr)
{
    int index = (int)parameter;
    int de_tap;
    int de_index;
    if (index < GdFirstParameterOfFirstTap) {
        de_index = index;
        de_tap = -1;
    }
    else {
        de_index = (index - GdFirstParameterOfFirstTap) % GdNumPametersPerTap + GdFirstParameterOfFirstTap;
        de_tap = (index - GdFirstParameterOfFirstTap) / GdNumPametersPerTap;
    }
    if (de_tap_ptr)
        *de_tap_ptr = de_tap;
    return (GdParameter)de_index;
}

inline GdParameter GdRecomposeParameter(GdParameter de_parameter, int de_tap)
{
    int index;
    int de_index = (int)de_parameter;
    if (de_index < GdFirstParameterOfFirstTap)
        index = de_index;
    else
        index = de_index + de_tap * GdNumPametersPerTap;
    return (GdParameter)index;
}

// Accepted values for grid divisors
Ignorable static constexpr int GdGridDivisors[] = {4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128};
enum {
    GdNumGridDivisors = sizeof(GdGridDivisors) / sizeof(GdGridDivisors[0]),
    GdDefaultDivisor = 4,
    GdMinDivisor = GdGridDivisors[0],
    GdMaxDivisor = GdGridDivisors[GdNumGridDivisors - 1],
};

// Get the nearest accepted grid divisor
int GdFindNearestDivisor(float div);

// Get the grid interval, according to the grid division and tempo
inline float GdGetGridInterval(int div, float bpm)
{
    return 240.0f / ((float)div * bpm);
}

// Get the nearest delay value, aligned according to the grid division
inline float GdAlignDelayToGrid(float delay, int div, float bpm)
{
    float interval = GdGetGridInterval(div, bpm);
    delay = (delay > 0) ? delay : 0;
    int ndiv = (int)(delay / interval + 0.5f);
    delay = interval * (float)ndiv;
    delay = (delay < (float)GdMaxDelay) ? delay : (float)GdMaxDelay;
    return delay;
}
