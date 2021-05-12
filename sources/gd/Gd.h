// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "GdDefs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(GD_API)
#   define GD_API
#endif

typedef struct Gd Gd;

GD_API Gd *GdNew(unsigned numinputs, unsigned numoutputs);
GD_API void GdFree(Gd *gd);
GD_API void GdClear(Gd *gd);
GD_API void GdSetSampleRate(Gd *gd, float samplerate);
GD_API void GdSetBufferSize(Gd *gd, unsigned bufsize);
GD_API void GdProcess(Gd *gd, const float *inputs[], float *outputs[], unsigned count);
GD_API void GdSetParameter(Gd *gd, GdParameter p, float value);
GD_API void GdSetParameterEx(Gd *gd, GdParameter p, float value, bool force);
GD_API float GdGetParameter(Gd *gd, GdParameter p);
GD_API float GdAdjustParameter(GdParameter p, float value);
GD_API unsigned GdParameterCount();
GD_API const char *GdParameterName(GdParameter p);
GD_API GdParameter GdParameterByName(const char *name);
GD_API float GdParameterMin(GdParameter p);
GD_API float GdParameterMax(GdParameter p);
GD_API float GdParameterDefault(GdParameter p);
GD_API unsigned GdParameterFlags(GdParameter p);
GD_API const char *GdParameterLabel(GdParameter p);
GD_API const char *const *GdParameterChoices(GdParameter p);
GD_API int GdParameterGroup(GdParameter p);
GD_API const char *GdGroupName(GdParameter p);
GD_API const char *GdGroupLabel(GdParameter p);

#if defined(__cplusplus)
} // extern "C"
#endif

#if defined(__cplusplus)
#   include <memory>

struct GdDelete;
using GdPtr = std::unique_ptr<Gd, GdDelete>;

struct GdDelete {
    void operator()(Gd *gd) const noexcept
    {
        GdFree(gd);
    }
};
#endif
