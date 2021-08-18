#include "GdShifter.h"

#if GD_USE_SOUNDTOUCH_SHIFTER
#include "shifters/GdShifterSoundTouch.cpp"
#else
#include "shifters/GdShifterSimple.cpp"
#endif
