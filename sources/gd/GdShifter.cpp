#include "GdShifter.h"

#if GD_USE_SOUNDTOUCH_PITCH_SHIFTER
#include "shifters/GdShifterSoundTouch.cpp"
#elif GD_USE_SIMPLE_PITCH_SHIFTER
#include "shifters/GdShifterSimple.cpp"
#endif
