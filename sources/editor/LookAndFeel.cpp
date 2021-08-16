#include "editor/LookAndFeel.h"
#include "editor/parts/TapEditScreen.h"
#include "editor/parts/TapSlider.h"
#include "BinaryData.h"

static const juce::StringRef kSansSerifTypefaceName = "Liberation Sans";

struct LookAndFeel::Impl {
    juce::Typeface::Ptr sansTypeface_;
};

LookAndFeel::LookAndFeel()
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.sansTypeface_ = juce::Typeface::createSystemTypefaceFor(BinaryData::LiberationSansRegular_ttf, BinaryData::LiberationSansRegular_ttfSize);

    ///
    setColourScheme(getLightColourScheme());
    setColour(TapEditScreen::lineColourId, juce::Colour(0xff, 0xff, 0xff));
    setColour(TapEditScreen::tapLineColourId, juce::Colour(0xff, 0xff, 0x80));
}

LookAndFeel::~LookAndFeel()
{
}

juce::Typeface::Ptr LookAndFeel::getTypefaceForFont(const juce::Font &font)
{
    Impl &impl = *impl_;
    juce::Typeface::Ptr tf;

    const juce::String &typefaceName = font.getTypefaceName();
    if (typefaceName == juce::Font::getDefaultSansSerifFontName())
        tf = impl.sansTypeface_;
    else if (typefaceName == kSansSerifTypefaceName)
        tf = impl.sansTypeface_;

    if (!tf)
        tf = BaseLookAndFeel::getTypefaceForFont(font);

    return tf;
}

juce::Slider::SliderLayout LookAndFeel::getSliderLayout(juce::Slider &slider)
{
    const juce::NamedValueSet &properties = slider.getProperties();

    if (const juce::var *xSliderClass = properties.getVarPointer("X-Slider-Class")) {
        if (*xSliderClass == "TapSlider") {
            juce::Slider::SliderLayout layout;
            layout.sliderBounds = slider.getLocalBounds().reduced(1, 1);
            return layout;
        }
    }

    return BaseLookAndFeel::getSliderLayout(slider);
}
