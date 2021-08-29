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

#if GD_USE_SOUNDTOUCH_PITCH_SHIFTER
#   include "shifters/GdShifterSoundTouch.h"
#elif GD_USE_SIMPLE_PITCH_SHIFTER
#   include "shifters/GdShifterSimple.h"
#elif GD_USE_SUPERCOLLIDER_PITCH_SHIFTER
#   include "shifters/GdShifterSuperCollider.h"
#else
#   error No pitch shifter selected
#endif

#if !defined(GD_SHIFTER_UPDATES_AT_K_RATE)
#   error Must define GD_SHIFTER_UPDATES_AT_K_RATE
#endif
#if !defined(GD_SHIFTER_CAN_REPORT_LATENCY)
#   error Must define GD_SHIFTER_CAN_REPORT_LATENCY
#endif
