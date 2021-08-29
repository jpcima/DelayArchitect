/* Copyright (c) 2021, Jean Pierre Cimalando
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "editor/LookAndFeel.h"
#include "editor/parts/TapEditScreen.h"
#include "editor/parts/TapSlider.h"
#include "BinaryData.h"
#include <fontaudio/fontaudio.h>

static const juce::StringRef kSansSerifTypefaceName = "Liberation Sans";

struct LookAndFeel::Impl {
    juce::Typeface::Ptr sansTypeface_;
    juce::SharedResourcePointer<fontaudio::IconHelper> fontAudio_;
};

LookAndFeel::LookAndFeel()
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.sansTypeface_ = juce::Typeface::createSystemTypefaceFor(BinaryData::LiberationSansRegular_ttf, BinaryData::LiberationSansRegular_ttfSize);

    ///
    setColourScheme(getLightColourScheme());
    setColour(TapEditScreen::lineColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::screenContourColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::intervalFillColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.25f));
    setColour(TapEditScreen::intervalContourColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::tapLineColourId, juce::Colour(0xff, 0xff, 0x80));
    setColour(TapEditScreen::tapLabelBackgroundColourId, juce::Colour(0xff, 0xff, 0xff));
    setColour(TapEditScreen::tapLabelSelectedBackgroundColourId, juce::Colour(0xff, 0xff, 0x80));
    setColour(TapEditScreen::tapLabelTextColourId, juce::Colour(0x00, 0x00, 0x00));
    setColour(TapEditScreen::minorIntervalTickColourId, juce::Colour(0x00, 0x00, 0x00).withAlpha(0.5f));
    setColour(TapEditScreen::majorIntervalTickColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::superMajorIntervalTickColourId, juce::Colour(0xff, 0xff, 0xff));
    setColour(TapEditScreen::lassoFillColourId, juce::Colour(0x00, 0x00, 0x00).withAlpha(0.25f));
    setColour(TapEditScreen::lassoOutlineColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.25f));
    setColour(TapEditScreen::textColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.75f));
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
    else if (typefaceName == "Fontaudio")
        tf = impl.fontAudio_->getFont().getTypeface();

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

void LookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    // NOTE(jpc) reduce the size of the arrow zone
    juce::Rectangle<int> arrowZone(width - 20/*30*/, 0, 14/*20*/, height);
    juce::Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

    g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void LookAndFeel::positionComboBoxText(juce::ComboBox &box, juce::Label &label)
{
    // NOTE(jpc) reduce the size of the arrow zone
    label.setBounds(1, 1,
                    box.getWidth() - 20/*30*/,
                    box.getHeight() - 2);

    label.setFont(getComboBoxFont(box));
}

void LookAndFeel::setTextButtonFont(juce::TextButton &button, const juce::Font &font)
{
    juce::NamedValueSet &properties = button.getProperties();
    properties.set("font", font.getTypefaceName());
    properties.set("font-size", font.getHeight());
    properties.set("font-style", font.getStyleFlags());
}

juce::Font LookAndFeel::getTextButtonFont(juce::TextButton &button, int buttonHeight)
{
    const juce::NamedValueSet &properties = button.getProperties();
    if (properties.contains("font")) {
        return juce::Font(
            properties.getWithDefault("font", "").toString(),
            (float)properties.getWithDefault("font-size", 0.0f),
            (int)properties.getWithDefault("font-style", 0));
    }
    return BaseLookAndFeel::getTextButtonFont(button, buttonHeight);
}

void LookAndFeel::setComboBoxFont(juce::ComboBox &combo, const juce::Font &font)
{
    juce::NamedValueSet &properties = combo.getProperties();
    properties.set("font", font.getTypefaceName());
    properties.set("font-size", font.getHeight());
    properties.set("font-style", font.getStyleFlags());
}

juce::Font LookAndFeel::getComboBoxFont(juce::ComboBox &combo)
{
    const juce::NamedValueSet &properties = combo.getProperties();
    if (properties.contains("font")) {
        return juce::Font(
            properties.getWithDefault("font", "").toString(),
            (float)properties.getWithDefault("font-size", 0.0f),
            (int)properties.getWithDefault("font-style", 0));
    }
    return BaseLookAndFeel::getComboBoxFont(combo);
}
