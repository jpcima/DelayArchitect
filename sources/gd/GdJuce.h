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
#include <juce_core/juce_core.h>
#include "GdDefs.h"

template <class Value>
juce::NormalisableRange<Value> GdJuceRange(const GdRange &range)
{
    juce::NormalisableRange<Value> normalisedRange{(Value)range.start, (Value)range.end, (Value)range.interval};
    switch (range.mode) {
    default:
    case GDR_NONE:
        normalisedRange.skew = (Value)range.skew;
        normalisedRange.symmetricSkew = false;
        break;
    case GDR_SYMMETRIC:
        normalisedRange.skew = (Value)range.skew;
        normalisedRange.symmetricSkew = true;
        break;
    case GDR_MIDPOINT:
        normalisedRange.setSkewForCentre((Value)range.skew);
        break;
    }
    return normalisedRange;
}
