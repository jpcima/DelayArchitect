#include "TapSlider.h"

struct TapSlider::Impl {
};

TapSlider::TapSlider()
    : impl_(new Impl)
{
    setSliderStyle(LinearBarVertical);
    setTextBoxStyle(NoTextBox, true, 0, 0);
    setRange(-1.0, 1.0);
}

TapSlider::~TapSlider()
{
}

void TapSlider::paint(juce::Graphics &g)
{
    juce::LookAndFeel &lnf = getLookAndFeel();
    juce::Slider::SliderLayout layout = lnf.getSliderLayout(*this);

    juce::Rectangle<int> sliderRect = layout.sliderBounds;
    float lastCurrentValue = (float)getValue();
    float sliderPos = getPositionOfValue(lastCurrentValue);
    int x = sliderRect.getX();
    int y = sliderRect.getY();
    int width = sliderRect.getWidth();
    int height = sliderRect.getHeight();

    g.setColour(findColour(backgroundColourId));
    g.fillRect(sliderRect);

    g.setColour(findColour(trackColourId));
    g.fillRect(isHorizontal() ? juce::Rectangle<float>((float)x, (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
               : juce::Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));
}
