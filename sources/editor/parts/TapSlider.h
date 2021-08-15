#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

class TapSlider : public juce::Slider {
public:
    TapSlider();
    ~TapSlider() override;

protected:
    void paint(juce::Graphics &g) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
