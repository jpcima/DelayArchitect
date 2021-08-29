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
