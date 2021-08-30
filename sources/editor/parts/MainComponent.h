/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.1.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --

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

#include "editor/parts/BetterSlider.h"
#include <GdDefs.h>
#include <juce_gui_basics/juce_gui_basics.h>
class TapEditScreen;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainComponent  : public juce::Component,
                       public juce::Button::Listener,
                       public juce::Slider::Listener,
                       public juce::ComboBox::Listener
{
public:
    //==============================================================================
    MainComponent ();
    ~MainComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    struct Impl;
    std::unique_ptr<Impl> impl_;

    public: /* forcing public visibility on member components
               very illegal programming practice. call the police */

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TapEditScreen> tapEditScreen_;
    std::unique_ptr<juce::TextButton> cutoffButton_;
    std::unique_ptr<juce::TextButton> resonanceButton_;
    std::unique_ptr<juce::TextButton> tuneButton_;
    std::unique_ptr<juce::TextButton> panButton_;
    std::unique_ptr<juce::TextButton> levelButton_;
    std::unique_ptr<juce::TextButton> firstTapButton_;
    std::unique_ptr<juce::TextButton> lastTapButton_;
    std::unique_ptr<better::Slider> tapDelaySlider_;
    std::unique_ptr<juce::Label> unknown;
    std::unique_ptr<juce::TextButton> tapEnabledButton_;
    std::unique_ptr<juce::ComboBox> feedbackTapChoice_;
    std::unique_ptr<juce::Label> unknown2;
    std::unique_ptr<better::Slider> feedbackTapGainSlider_;
    std::unique_ptr<juce::Label> unknown3;
    std::unique_ptr<better::Slider> drySlider_;
    std::unique_ptr<better::Slider> wetSlider_;
    std::unique_ptr<juce::ComboBox> filterChoice_;
    std::unique_ptr<juce::Label> unknown4;
    std::unique_ptr<better::Slider> hpfCutoffSlider_;
    std::unique_ptr<better::Slider> lpfCutoffSlider_;
    std::unique_ptr<juce::Label> unknown5;
    std::unique_ptr<juce::Label> unknown6;
    std::unique_ptr<better::Slider> resonanceSlider_;
    std::unique_ptr<juce::Label> unknown7;
    std::unique_ptr<better::Slider> pitchSlider_;
    std::unique_ptr<juce::Label> unknown8;
    std::unique_ptr<better::Slider> panSlider_;
    std::unique_ptr<juce::Label> unknown9;
    std::unique_ptr<better::Slider> widthSlider_;
    std::unique_ptr<juce::Label> unknown10;
    std::unique_ptr<better::Slider> levelSlider_;
    std::unique_ptr<juce::Label> unknown11;
    std::unique_ptr<juce::Label> unknown12;
    std::unique_ptr<juce::Label> unknown13;
    std::unique_ptr<juce::TextButton> syncButton_;
    std::unique_ptr<juce::ComboBox> gridChoice_;
    std::unique_ptr<juce::Label> unknown14;
    std::unique_ptr<juce::TextButton> menuButton_;
    std::unique_ptr<juce::Label> unknown15;
    std::unique_ptr<better::Slider> swingSlider_;
    std::unique_ptr<juce::TextButton> filterEnableButton_;
    std::unique_ptr<juce::TextButton> tuneEnableButton_;
    std::unique_ptr<juce::TextButton> muteButton_;
    std::unique_ptr<juce::TextButton> feedbackEnableButton_;
    std::unique_ptr<juce::TextButton> flipEnableButton_;
    std::unique_ptr<juce::ComboBox> activeTapChoice_;
    std::unique_ptr<juce::TextButton> tapMenuButton_;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

