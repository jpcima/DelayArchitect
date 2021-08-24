#pragma once
#include "Importer.h"

class ImporterPST : public Importer {
public:
    bool importFile(const juce::File &file, ImportData &idata) override;
};
