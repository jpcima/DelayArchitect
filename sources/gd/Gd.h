/* Delay Architect
 * Copyright (C) 2021  Jean Pierre Cimalando
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
GD_API void GdSetTempo(Gd *gd, float tempo);
GD_API void GdSetParameter(Gd *gd, GdParameter p, float value);
GD_API void GdSetParameterEx(Gd *gd, GdParameter p, float value, bool force);
GD_API float GdGetParameter(Gd *gd, GdParameter p);
GD_API float GdAdjustParameter(GdParameter p, float value);
GD_API unsigned GdParameterCount();
GD_API const char *GdParameterName(GdParameter p);
GD_API GdParameter GdParameterByName(const char *name);
GD_API GdRange GdParameterRange(GdParameter p);
GD_API float GdParameterMin(GdParameter p);
GD_API float GdParameterMax(GdParameter p);
GD_API float GdParameterDefault(GdParameter p);
GD_API unsigned GdParameterFlags(GdParameter p);
GD_API const char *GdParameterLabel(GdParameter p);
GD_API const char *GdParameterUnit(GdParameter p);
GD_API const char *const *GdParameterChoices(GdParameter p);
GD_API int GdParameterGroup(GdParameter p);
GD_API const char *GdGroupName(GdParameter p);
GD_API const char *GdGroupLabel(GdParameter p);
GD_API void GdFormatParameterValue(GdParameter p, float value, char *text, unsigned textsize);

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
