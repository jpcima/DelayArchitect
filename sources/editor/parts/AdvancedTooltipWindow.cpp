#include "AdvancedTooltipWindow.h"

void AdvancedTooltipWindow::setPersistentText(const juce::String &text)
{
    persistentText_ = text;
    hasPersistentText_ = true;
}

void AdvancedTooltipWindow::clearPersistentText()
{
    hasPersistentText_ = false;
}

juce::String AdvancedTooltipWindow::getTipFor(juce::Component &component)
{
    if (hasPersistentText_)
        return persistentText_;

    return juce::TooltipWindow::getTipFor(component);
}
