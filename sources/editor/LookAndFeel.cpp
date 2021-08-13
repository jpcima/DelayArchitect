#include "editor/LookAndFeel.h"
#include "editor/parts/TapEditScreen.h"
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
