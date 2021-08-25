#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class AdvancedTooltipWindow final : public juce::TooltipWindow {
public:
    using juce::TooltipWindow::TooltipWindow;

    void setPersistentText(const juce::String &text);
    void clearPersistentText();

    juce::String getTipFor(juce::Component &component) override;

private:
    bool hasPersistentText_ = false;
    juce::String persistentText_;
};
