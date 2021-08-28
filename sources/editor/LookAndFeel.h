#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

using BaseLookAndFeel = juce::LookAndFeel_V4;

class LookAndFeel final : public BaseLookAndFeel {
public:
    LookAndFeel();
    ~LookAndFeel() override;

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font &font) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override;

    void drawComboBox(juce::Graphics &, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &) override;
    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override;

    static void setTextButtonFont(juce::TextButton &button, const juce::Font &font);
    juce::Font getTextButtonFont(juce::TextButton &button, int buttonHeight) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
