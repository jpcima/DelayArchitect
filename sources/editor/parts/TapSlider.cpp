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

    juce::NamedValueSet &properties = getProperties();
    properties.set("X-Slider-Class", "TapSlider");
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
    int numberOfValues{};

    bool isSupportedSliderStyle;
    switch (sliderStyle) {
    case LinearHorizontal: case LinearVertical: case LinearBarVertical:
        numberOfValues = 1;
        isSupportedSliderStyle = true;
        break;
    case TwoValueVertical: case TwoValueHorizontal:
        numberOfValues = 2;
        isSupportedSliderStyle = true;
        break;
    default:
        // not supported
        return;
    }

    Impl &impl = *impl_;
    juce::LookAndFeel &lnf = getLookAndFeel();
    juce::Slider::SliderLayout layout = lnf.getSliderLayout(*this);
    bool isBipolar = impl.isBipolar_;

    juce::Rectangle<int> sliderRect = layout.sliderBounds;
    float lastCurrentValue{};
    float lastCurrentValue2{};
    float sliderPos{};
    float sliderPos2{};
    if (numberOfValues == 1) {
        lastCurrentValue = (float)getValue();
        sliderPos = getPositionOfValue(lastCurrentValue);
    }
    else if (numberOfValues == 2) {
        lastCurrentValue = (float)getMinValue();
        lastCurrentValue2 = (float)getMaxValue();
        sliderPos = getPositionOfValue(lastCurrentValue);
        sliderPos2 = getPositionOfValue(lastCurrentValue2);
    }
    int x = sliderRect.getX();
    int y = sliderRect.getY();
    int width = sliderRect.getWidth();
    int height = sliderRect.getHeight();

    g.setColour(findColour(backgroundColourId));
    g.fillRect(sliderRect);

    juce::Rectangle<float> filledRect;
    if (isHorizontal()) {
        float fillX{};
        float fillWidth{};
        if (numberOfValues == 1 && !isBipolar) {
            fillX = (float)x;
            fillWidth = sliderPos - (float)x;
        }
        else if (numberOfValues == 1 && isBipolar) {
            float centerPos = getPositionOfValue(impl.centerValue_);
            fillX = (sliderPos < centerPos) ? sliderPos : centerPos;
            fillWidth = ((sliderPos < centerPos) ? centerPos : sliderPos) - fillX;
        }
        else if (numberOfValues == 2) {
            fillX = sliderPos;
            fillWidth = sliderPos2 - (float)sliderPos;
        }
        if (fillWidth > 0)
            filledRect = juce::Rectangle<float>(fillX, (float)y + 0.5f, fillWidth, (float)height - 1.0f);
    }
    else {
        float fillY{};
        float fillHeight{};
        if (numberOfValues == 1 && !isBipolar) {
            fillY = sliderPos;
            fillHeight = (float)y + ((float)height - sliderPos);
        }
        else if (numberOfValues == 1 && isBipolar) {
            float centerPos = getPositionOfValue(impl.centerValue_);
            fillY = (sliderPos < centerPos) ? sliderPos : centerPos;
            fillHeight = ((sliderPos < centerPos) ? centerPos : sliderPos) - fillY;
        }
        else if (numberOfValues == 2) {
            fillY = sliderPos2;
            fillHeight = sliderPos - fillY;
        }
        if (fillHeight > 0)
            filledRect = juce::Rectangle<float>((float)x + 0.5f, fillY, (float)width - 1.0f, fillHeight);
    }

    g.setColour(findColour(trackColourId));
    g.fillRect(filledRect);
}
