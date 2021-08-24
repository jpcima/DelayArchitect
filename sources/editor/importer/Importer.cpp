#include "Importer.h"
#include <Gd.h>
#include <iomanip>
#include <sstream>

juce::String Importer::toString(const ImportData &idata)
{
    std::ostringstream stream(std::ios::binary);

    for (uint32_t p = 0; p < GD_PARAMETER_COUNT; ++p) {
        int tapNumber;
        GdDecomposeParameter((GdParameter)p, &tapNumber);
        if (tapNumber != -1) {
            GdParameter enable = GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber);
            if (!idata.values[(int)enable])
                continue;
        }
        const char *label = GdParameterLabel((GdParameter)p);
        stream << std::setw(32) << (juce::String(label) + ": ")
               << idata.values[p] << "\n";
    }

    return stream.str();
}
