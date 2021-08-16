#include "TapSlider.h"

struct TapSlider::Impl {
    bool isBipolar_ = false;
    float centerValue_ = 0;
};

TapSlider::TapSlider()
    : impl_(new Impl)
{
    setSliderStyle(LinearBarVertical);
    setTextBoxStyle(NoTextBox, true, 0, 0);
}

TapSlider::~TapSlider()
{
}

void TapSlider::setBipolarAround(bool isBipolar, float centerValue)
{
    Impl &impl = *impl_;

    if (impl.isBipolar_ == isBipolar && impl.centerValue_ == centerValue)
        return;

    impl.isBipolar_ = isBipolar;
    impl.centerValue_ = centerValue;
    repaint();
}

void TapSlider::paint(juce::Graphics &g)
{
    int sliderStyle = getSliderStyle();
    if (sliderStyle != LinearBarVertical) {
        #pragma message("TODO: implement the two-value slider")
        Slider::paint(g);
        return;
    }

    Impl &impl = *impl_;
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

    juce::Rectangle<float> filledRect;
    if (isHorizontal()) {
        float fillX, fillWidth;
        if (!impl.isBipolar_) {
            fillX = (float)x;
            fillWidth = sliderPos - (float)x;
        }
        else {
            float centerPos = getPositionOfValue(impl.centerValue_);
            fillX = (sliderPos < centerPos) ? sliderPos : centerPos;
            fillWidth = ((sliderPos < centerPos) ? centerPos : sliderPos) - fillX;
        }
        filledRect = juce::Rectangle<float>(fillX, (float)y + 0.5f, fillWidth, (float)height - 1.0f);
    }
    else {
        float fillY, fillHeight;
        if (!impl.isBipolar_) {
            fillY = sliderPos;
            fillHeight = (float)y + ((float)height - sliderPos);
        }
        else {
            float centerPos = getPositionOfValue(impl.centerValue_);
            fillY = (sliderPos < centerPos) ? sliderPos : centerPos;
            fillHeight = ((sliderPos < centerPos) ? centerPos : sliderPos) - fillY;
        }
        filledRect = juce::Rectangle<float>((float)x + 0.5f, fillY, (float)width - 1.0f, fillHeight);
    }

    g.setColour(findColour(trackColourId));
    g.fillRect(filledRect);
}
