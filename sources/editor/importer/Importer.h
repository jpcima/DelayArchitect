#pragma once
#include "GdDefs.h"
#include <juce_core/juce_core.h>

struct ImportData {
    float values[GD_PARAMETER_COUNT] {};
};

class Importer {
public:
    virtual ~Importer() {}
    virtual bool importFile(const juce::File &file, ImportData &idata) = 0;
    static juce::String toString(const ImportData &idata);
};
