#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

using BaseLookAndFeel = juce::LookAndFeel_V4;

class LookAndFeel final : public BaseLookAndFeel {
public:
    LookAndFeel();
    ~LookAndFeel() override;

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font &font) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
