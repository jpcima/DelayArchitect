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

#include "FadGlyphButton.h"
#include <fontaudio/fontaudio.h>

struct FadGlyphButton::Impl {
    FadGlyphButton *self_ = nullptr;
    float currentGlyphHeight_ = 0.0f;
    void updateGlyphBorder();
};

FadGlyphButton::FadGlyphButton(const juce::String &name)
    : juce::ShapeButton(name, juce::Colour{}, juce::Colour{}, juce::Colour{}),
      impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    juce::Colour on = findColour(juce::TextButton::buttonOnColourId);
    juce::Colour off = findColour(juce::TextButton::textColourOffId);
    juce::Colour hlOn = on.brighter();
    juce::Colour hlOff = off.darker();
    setColours(off, hlOff, on);
    setOnColours(on, hlOn, off);
    shouldUseOnColours(true);
}

FadGlyphButton::~FadGlyphButton()
{
}

void FadGlyphButton::setIcon(const fontaudio::IconName &iconName, float glyphHeight, bool resizeNowToFitThisShape, bool maintainShapeProportions, bool hasDropShadow)
{
    juce::juce_wchar utf32[2] {};
    iconName.copyToUTF32(utf32, sizeof(utf32));
    return setGlyph(utf32[0], glyphHeight, resizeNowToFitThisShape, maintainShapeProportions, hasDropShadow);
}

void FadGlyphButton::setGlyph(juce::juce_wchar codePoint, float glyphHeight, bool resizeNowToFitThisShape, bool maintainShapeProportions, bool hasDropShadow)
{
    juce::Path path;
    {
        juce::Font font("Fontaudio", glyphHeight, juce::Font::plain);
        juce::GlyphArrangement ga;
        ga.addLineOfText(font, juce::String::charToString(codePoint), 0.0f, 0.0f);
        ga.createPath(path);
    }

    setShape(path, resizeNowToFitThisShape, maintainShapeProportions, hasDropShadow);

    Impl &impl = *impl_;
    impl.currentGlyphHeight_ = glyphHeight;
    impl.updateGlyphBorder();
}

void FadGlyphButton::resized()
{
    Impl &impl = *impl_;
    impl.updateGlyphBorder();
}

void FadGlyphButton::Impl::updateGlyphBorder()
{
    FadGlyphButton *self = self_;
    float gh = currentGlyphHeight_;
    int vgap = juce::roundToInt(((float)self->getHeight() - gh) / 2.0f);
    self->setBorderSize(juce::BorderSize<int>{vgap, 0, vgap, 0});
}
