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

#pragma once
#include "editor/parts/TapEditScreen.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

using BaseLookAndFeel = juce::LookAndFeel_V4;

class LookAndFeel final : public BaseLookAndFeel,
                          public TapEditScreen::LookAndFeelMethods {
public:
    LookAndFeel();
    ~LookAndFeel() override;

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font &font) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override;
    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider &slider) override;
    void drawLinearSliderBackground(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider &slider) override;
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override;
    int getSliderThumbRadius(juce::Slider &slider) override;
    juce::Label *createSliderTextBox(juce::Slider &slider) override;

    void drawComboBox(juce::Graphics &, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &) override;
    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override;

    static void setTextButtonFont(juce::TextButton &button, const juce::Font &font);
    juce::Font getTextButtonFont(juce::TextButton &button, int buttonHeight) override;

    static void setComboBoxFont(juce::ComboBox &combo, const juce::Font &font);
    juce::Font getComboBoxFont(juce::ComboBox &combo) override;

    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox &combo, juce::Label &label) override;

    void drawTooltip(juce::Graphics &g, const juce::String &text, int width, int height) override;

    juce::MouseCursor createPencilCursor() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
