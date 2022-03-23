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
#include <cmath>

static const juce::StringRef kSansSerifTypefaceName = "Liberation Sans";

struct LookAndFeel::Impl {
    juce::Typeface::Ptr sansTypeface_;
    juce::Typeface::Ptr sansBoldTypeface_;
    juce::SharedResourcePointer<fontaudio::IconHelper> fontAudio_;
};

LookAndFeel::LookAndFeel()
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.sansTypeface_ = juce::Typeface::createSystemTypefaceFor(BinaryData::LiberationSansRegular_ttf, BinaryData::LiberationSansRegular_ttfSize);
    impl.sansBoldTypeface_ = juce::Typeface::createSystemTypefaceFor(BinaryData::LiberationSansBold_ttf, BinaryData::LiberationSansBold_ttfSize);

    ///
    static_assert(BaseLookAndFeel::ColourScheme::numColours == 9, "unexpected number of colors");

    BaseLookAndFeel::ColourScheme cs = {
        /* windowBackground */  0xff2f2f3au,
        /* widgetBackground */  0xff262626u /*0xff191926u*/,
        /* menuBackground */    0xffd0d0d0u,
        /* outline */           0x4066667cu /*0xff66667cu*/,
        /* defaultText */       0xc0ffffffu /*0xc8ffffffu*/,
        /* defaultFill */       0xffd8d8d8u,
        /* highlightedText */   0xffffffffu,
        /* highlightedFill */   0xffecb336u /*0xff606073u*/,
        /* menuText */          0xff000000u,
    };

    setColourScheme(cs);

    // colour scheme overrides
    setColour(juce::TextButton::textColourOnId, juce::Colour(0x00, 0x00, 0x00));
    setColour(juce::TooltipWindow::textColourId, juce::Colour(0x00, 0x00, 0x00));
    setColour(juce::TooltipWindow::outlineColourId, juce::Colour(0x00, 0x00, 0x00));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour(0x00, 0x00, 0x00));

    ///
    setColour(TapEditScreen::lineColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::screenContourColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::intervalFillColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.25f));
    setColour(TapEditScreen::intervalContourColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::tapLineColourId, juce::Colour(0xff, 0xff, 0x80));
    setColour(TapEditScreen::tapLabelTextColourId, juce::Colour(0x00, 0x00, 0x00));
    setColour(TapEditScreen::tapSliderBackgroundColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.25f));
    setColour(TapEditScreen::minorIntervalTickColourId, juce::Colour(0x00, 0x00, 0x00).withAlpha(0.5f));
    setColour(TapEditScreen::majorIntervalTickColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.5f));
    setColour(TapEditScreen::superMajorIntervalTickColourId, juce::Colour(0xff, 0xff, 0xff));
    setColour(TapEditScreen::lassoFillColourId, juce::Colour(0x00, 0x00, 0x00).withAlpha(0.25f));
    setColour(TapEditScreen::lassoOutlineColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.25f));
    setColour(TapEditScreen::textColourId, juce::Colour(0xff, 0xff, 0xff).withAlpha(0.75f));
    setColour(TapEditScreen::editCutoffBaseColourId, juce::Colour(0xd8, 0x76, 0x7b));
    setColour(TapEditScreen::editResonanceBaseColourId, juce::Colour(0xd4, 0xe0, 0x7d));
    setColour(TapEditScreen::editTuneBaseColourId, juce::Colour(0x7a, 0xe1, 0x84));
    setColour(TapEditScreen::editPanBaseColourId, juce::Colour(0xd8, 0x70, 0xde));
    setColour(TapEditScreen::editLevelBaseColourId, juce::Colour(0x7a, 0xdd, 0xdf));
    // left pan #138EBC
    // right pan #179F64
}

LookAndFeel::~LookAndFeel()
{
}

juce::Typeface::Ptr LookAndFeel::getTypefaceForFont(const juce::Font &font)
{
    Impl &impl = *impl_;
    juce::Typeface::Ptr tf;

    const juce::String &typefaceName = font.getTypefaceName();
    if (typefaceName == juce::Font::getDefaultSansSerifFontName() ||
        typefaceName == kSansSerifTypefaceName)
    {
        if (font.isBold())
            tf = impl.sansBoldTypeface_;
        else
            tf = impl.sansTypeface_;
    }
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

    if (slider.isRotary()) {
        juce::Slider::SliderLayout layout = BaseLookAndFeel::getSliderLayout(slider);
        layout.sliderBounds.setHeight(layout.sliderBounds.getHeight() + layout.textBoxBounds.getHeight() / 2);
        return layout;
    }

    return BaseLookAndFeel::getSliderLayout(slider);
}

void LookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider &slider)
{
    if (style == juce::Slider::LinearBar) {
        float fx = (float)x;
        float fy = (float)y;
        float fw = (float)width;
        float fh = (float)height;

        float bh = 3.0f;
        float br = 3.0f;
        juce::Rectangle<float> wholeBar{fx, fy + fh - bh, fw, bh};
        juce::Rectangle<float> filledBar{sliderPos, fy + fh - bh, fw, bh};

        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.fillRoundedRectangle(wholeBar, br);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.fillRoundedRectangle(filledBar, br);

        return;
    }

    if (/*style == juce::Slider::LinearBar ||*/ style == juce::Slider::LinearBarVertical) {
        g.fillAll(slider.findColour(juce::Slider::backgroundColourId));
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>((float)x, (float)y, sliderPos - (float)x, (float)height)
                                          : juce::Rectangle<float>((float)x, sliderPos, (float)width, (float)y + ((float)height - sliderPos)));
        return;
    }

    if (style == juce::Slider::LinearHorizontal || style == juce::Slider::LinearVertical) {
        float trackWidth = juce::jmin(4.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
                                      slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
                                    slider.isHorizontal() ? startPoint.y : (float)y);

        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint;

        float kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
        float ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = { kx, ky };

        int thumbWidth = getSliderThumbRadius(slider);

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillEllipse(juce::Rectangle<float>((float)thumbWidth, (float)thumbWidth).withCentre(maxPoint));

        return;
    }

    BaseLookAndFeel::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
}

int LookAndFeel::getSliderThumbRadius(juce::Slider &slider)
{
    return juce::jmin(
        8, slider.isHorizontal() ?
        (int)((float)slider.getHeight() * 0.5f) :
        (int)((float)slider.getWidth()  * 0.5f));
}

void LookAndFeel::drawLinearSliderBackground(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider &slider)
{
    BaseLookAndFeel::drawLinearSliderBackground(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
}

void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    juce::Colour outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    juce::Colour fill = slider.findColour(juce::Slider::rotarySliderFillColourId);
    juce::Colour thumb = slider.findColour(juce::Slider::thumbColourId);

    juce::Rectangle<float> bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float lineW = juce::jmin(4.0f, radius * 0.5f);
    float arcRadius = radius - lineW * 0.5f;

    ///
    float bodyRadius = (1.5f * radius - lineW);
    g.setColour(thumb.darker());
    g.fillEllipse(bounds.withSizeKeepingCentre(bodyRadius, bodyRadius));
    g.setColour(thumb.withAlpha(0.5f));
    g.drawEllipse(bounds.withSizeKeepingCentre(bodyRadius, bodyRadius), 2.0f);

    ///
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                rotaryEndAngle,
                                true);

    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
                               bounds.getCentreY(),
                               arcRadius,
                               arcRadius,
                               0.0f,
                               rotaryStartAngle,
                               toAngle,
                               true);

        g.setColour(fill);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 2.0f;
    juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                  bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

    g.setColour(thumb);
    g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

juce::Label *LookAndFeel::createSliderTextBox(juce::Slider &slider)
{
    std::unique_ptr<juce::Label> label{BaseLookAndFeel::createSliderTextBox(slider)};
    label->setColour(juce::Label::outlineColourId, juce::Colour{});
    return label.release();
}

void LookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
}

void LookAndFeel::positionComboBoxText(juce::ComboBox &box, juce::Label &label)
{
    label.setBounds(1, 1, box.getWidth() - 2, box.getHeight() - 2);
    label.setFont(getComboBoxFont(box));
    label.setJustificationType(juce::Justification::horizontallyCentred);
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

juce::PopupMenu::Options LookAndFeel::getOptionsForComboBoxPopupMenu(juce::ComboBox &combo, juce::Label &label)
{
    return BaseLookAndFeel::getOptionsForComboBoxPopupMenu(combo, label)
        .withStandardItemHeight(0);
}

static juce::TextLayout layoutTooltipText(const juce::String &text, juce::Colour colour)
{
    const float tooltipFontSize = 13.0f;
    const int maxToolTipWidth = 400;

    juce::AttributedString s;
    s.setJustification(juce::Justification::centred);
    s.append(text, juce::Font(tooltipFontSize, juce::Font::bold), colour);

    juce::TextLayout tl;
    tl.createLayoutWithBalancedLineLengths(s, (float)maxToolTipWidth);
    return tl;
}

void LookAndFeel::drawTooltip(juce::Graphics &g, const juce::String &text, int width, int height)
{
    juce::Rectangle<int> bounds(width, height);

    g.setColour(findColour(juce::TooltipWindow::backgroundColourId));
    g.fillRect(bounds.toFloat());

    g.setColour(findColour(juce::TooltipWindow::outlineColourId));
    g.drawRect(bounds.toFloat());

    layoutTooltipText(text, findColour(juce::TooltipWindow::textColourId))
        .draw(g, { (float)width, (float)height });
}

juce::MouseCursor LookAndFeel::createPencilCursor()
{
    Impl &impl = *impl_;
    fontaudio::IconHelper &fad = *impl.fontAudio_;

    int height = 20;
    juce::Image img = fad.getIcon(fontaudio::Pen, (float)height, juce::Colours::white);
    juce::Point<int> hotspot{0, height - 1};

    return juce::MouseCursor(img, hotspot.getX(), hotspot.getY());
}
