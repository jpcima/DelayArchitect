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
    GdMaxDelay = 5,
};

// parameter smoothing time in seconds
Ignorable static constexpr float GdParamSmoothTime = 10e-3f;

// smallest gain setting
Ignorable static constexpr float GdMinGain = 1e-4f;

#define GD_EACH_PARAMETER(_)                                                   \
    /* Name, Min, Max, Def, Flags, Label, Group */                             \
    _(FEEDBACK_TAP, 0, GdMaxLines - 1, 0, GDP_CHOICE, "Feedback tap", -1)      \
    _(FEEDBACK_GAIN, GdMinGain, 1, 0, GDP_FLOAT|GDP_LOGARITHMIC, "Feedback gain", -1) \
    _(MIX_DRY, GdMinGain, 1, 0, GDP_FLOAT|GDP_LOGARITHMIC, "Dry mix", -1)      \
    _(MIX_WET, GdMinGain, 1, 0, GDP_FLOAT|GDP_LOGARITHMIC, "Wet mix", -1)      \
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
    _(TAP_##X##_ENABLE, false, true, false, GDP_BOOLEAN, "Tap " #X " Enable", I) \
    _(TAP_##X##_DELAY, 0, GdMaxDelay, 0, GDP_FLOAT, "Tap " #X " Delay", I)     \
    _(TAP_##X##_PAN, -1, 1, 0, GDP_FLOAT, "Tap " #X " Pan", I)                 \
    _(TAP_##X##_SPREAD, 0, 1, 0, GDP_FLOAT, "Tap " #X " Spread", I)            \
    /* End */

typedef enum GdParameter {
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
