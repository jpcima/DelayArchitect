#pragma once

#if GD_USE_SOUNDTOUCH_PITCH_SHIFTER
#   include "shifters/GdShifterSoundTouch.h"
#elif GD_USE_SIMPLE_PITCH_SHIFTER
#   include "shifters/GdShifterSimple.h"
#else
#   error No pitch shifter selected
#endif

#if !defined(GD_SHIFTER_UPDATES_AT_K_RATE)
#   error Must define GD_SHIFTER_UPDATES_AT_K_RATE
#endif
#if !defined(GD_SHIFTER_CAN_REPORT_LATENCY)
#   error Must define GD_SHIFTER_CAN_REPORT_LATENCY
#endif
