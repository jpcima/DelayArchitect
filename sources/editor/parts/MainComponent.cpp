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

//[Headers] You can add your own extra header files here...

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

#include "TapEditScreen.h"
#include "editor/LookAndFeel.h"
#include <fontaudio/fontaudio.h>
#include <array>
#include <cstdio>
//[/Headers]

#include "MainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
struct MainComponent::Impl : public TapEditScreen::Listener {
    MainComponent *self_ = nullptr;
    TapEditMode editMode_ = kTapEditOff;

    ///
    void setEditMode(TapEditMode editMode);

    ///
    void tappingHasStarted(TapEditScreen *) override;
    void tappingHasEnded(TapEditScreen *) override;
};
//[/MiscUserDefs]

//==============================================================================
MainComponent::MainComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    Impl &impl = *new Impl;
    impl_.reset(&impl);
    impl.self_ = this;
    //[/Constructor_pre]

    tapEditScreen_.reset (new TapEditScreen());
    addAndMakeVisible (tapEditScreen_.get());

    tapEditScreen_->setBounds (136, 40, 728, 384);

    cutoffButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (cutoffButton_.get());
    cutoffButton_->setButtonText (TRANS("Cutoff"));
    cutoffButton_->setConnectedEdges (juce::Button::ConnectedOnRight);
    cutoffButton_->addListener (this);

    cutoffButton_->setBounds (160, 8, 136, 24);

    resonanceButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (resonanceButton_.get());
    resonanceButton_->setButtonText (TRANS("Resonance"));
    resonanceButton_->setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    resonanceButton_->addListener (this);

    resonanceButton_->setBounds (296, 8, 136, 24);

    tuneButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (tuneButton_.get());
    tuneButton_->setButtonText (TRANS("Tune"));
    tuneButton_->setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    tuneButton_->addListener (this);

    tuneButton_->setBounds (432, 8, 136, 24);

    panButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (panButton_.get());
    panButton_->setButtonText (TRANS("Pan"));
    panButton_->setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    panButton_->addListener (this);

    panButton_->setBounds (568, 8, 136, 24);

    levelButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (levelButton_.get());
    levelButton_->setButtonText (TRANS("Level"));
    levelButton_->setConnectedEdges (juce::Button::ConnectedOnLeft);
    levelButton_->addListener (this);

    levelButton_->setBounds (704, 8, 136, 24);

    firstTapButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (firstTapButton_.get());
    firstTapButton_->setButtonText (TRANS("Start tap"));
    firstTapButton_->addListener (this);

    firstTapButton_->setBounds (16, 280, 104, 56);

    lastTapButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (lastTapButton_.get());
    lastTapButton_->setButtonText (TRANS("Last tap"));
    lastTapButton_->addListener (this);

    lastTapButton_->setBounds (16, 352, 104, 56);

    tapDelaySlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (tapDelaySlider_.get());
    tapDelaySlider_->setRange (0, 10, 0);
    tapDelaySlider_->setSliderStyle (juce::Slider::LinearBar);
    tapDelaySlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    tapDelaySlider_->addListener (this);

    tapDelaySlider_->setBounds (448, 544, 104, 24);

    unknown.reset (new juce::Label (juce::String(),
                                    TRANS("Delay")));
    addAndMakeVisible (unknown.get());
    unknown->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown->setJustificationType (juce::Justification::centred);
    unknown->setEditable (false, false, false);
    unknown->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown->setBounds (448, 520, 104, 24);

    tapEnabledButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (tapEnabledButton_.get());
    tapEnabledButton_->addListener (this);

    tapEnabledButton_->setBounds (472, 480, 56, 32);

    feedbackTapChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (feedbackTapChoice_.get());
    feedbackTapChoice_->setEditableText (false);
    feedbackTapChoice_->setJustificationType (juce::Justification::centredLeft);
    feedbackTapChoice_->setTextWhenNothingSelected (juce::String());
    feedbackTapChoice_->setTextWhenNoChoicesAvailable (juce::String());
    feedbackTapChoice_->addListener (this);

    feedbackTapChoice_->setBounds (904, 88, 80, 24);

    unknown2.reset (new juce::Label (juce::String(),
                                     TRANS("Feedback")));
    addAndMakeVisible (unknown2.get());
    unknown2->setFont (juce::Font (20.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown2->setJustificationType (juce::Justification::centred);
    unknown2->setEditable (false, false, false);
    unknown2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown2->setBounds (880, 48, 102, 24);

    feedbackTapGainSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (feedbackTapGainSlider_.get());
    feedbackTapGainSlider_->setRange (0, 10, 0);
    feedbackTapGainSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackTapGainSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    feedbackTapGainSlider_->addListener (this);

    feedbackTapGainSlider_->setBounds (880, 120, 104, 96);

    unknown3.reset (new juce::Label (juce::String(),
                                     TRANS("Mix")));
    addAndMakeVisible (unknown3.get());
    unknown3->setFont (juce::Font (20.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown3->setJustificationType (juce::Justification::centred);
    unknown3->setEditable (false, false, false);
    unknown3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown3->setBounds (880, 248, 102, 24);

    drySlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (drySlider_.get());
    drySlider_->setRange (0, 10, 0);
    drySlider_->setSliderStyle (juce::Slider::LinearVertical);
    drySlider_->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    drySlider_->addListener (this);

    drySlider_->setBounds (880, 288, 48, 128);

    wetSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (wetSlider_.get());
    wetSlider_->setRange (0, 10, 0);
    wetSlider_->setSliderStyle (juce::Slider::LinearVertical);
    wetSlider_->setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
    wetSlider_->addListener (this);

    wetSlider_->setBounds (936, 288, 48, 128);

    filterChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (filterChoice_.get());
    filterChoice_->setEditableText (false);
    filterChoice_->setJustificationType (juce::Justification::centredLeft);
    filterChoice_->setTextWhenNothingSelected (juce::String());
    filterChoice_->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    filterChoice_->addListener (this);

    filterChoice_->setBounds (40, 544, 128, 24);

    unknown4.reset (new juce::Label (juce::String(),
                                     TRANS("Filter")));
    addAndMakeVisible (unknown4.get());
    unknown4->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown4->setJustificationType (juce::Justification::centred);
    unknown4->setEditable (false, false, false);
    unknown4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown4->setBounds (16, 520, 152, 24);

    hpfCutoffSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (hpfCutoffSlider_.get());
    hpfCutoffSlider_->setRange (0, 10, 0);
    hpfCutoffSlider_->setSliderStyle (juce::Slider::LinearBar);
    hpfCutoffSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    hpfCutoffSlider_->addListener (this);

    hpfCutoffSlider_->setBounds (176, 544, 80, 24);

    lpfCutoffSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (lpfCutoffSlider_.get());
    lpfCutoffSlider_->setRange (0, 10, 0);
    lpfCutoffSlider_->setSliderStyle (juce::Slider::LinearBar);
    lpfCutoffSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    lpfCutoffSlider_->addListener (this);

    lpfCutoffSlider_->setBounds (264, 544, 80, 24);

    unknown5.reset (new juce::Label (juce::String(),
                                     TRANS("HPF cutoff")));
    addAndMakeVisible (unknown5.get());
    unknown5->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown5->setJustificationType (juce::Justification::centred);
    unknown5->setEditable (false, false, false);
    unknown5->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown5->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown5->setBounds (176, 520, 80, 24);

    unknown6.reset (new juce::Label (juce::String(),
                                     TRANS("LPF cutoff")));
    addAndMakeVisible (unknown6.get());
    unknown6->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown6->setJustificationType (juce::Justification::centred);
    unknown6->setEditable (false, false, false);
    unknown6->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown6->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown6->setBounds (264, 520, 80, 24);

    resonanceSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (resonanceSlider_.get());
    resonanceSlider_->setRange (0, 10, 0);
    resonanceSlider_->setSliderStyle (juce::Slider::LinearBar);
    resonanceSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    resonanceSlider_->addListener (this);

    resonanceSlider_->setBounds (352, 544, 80, 24);

    unknown7.reset (new juce::Label (juce::String(),
                                     TRANS("Resonance")));
    addAndMakeVisible (unknown7.get());
    unknown7->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown7->setJustificationType (juce::Justification::centred);
    unknown7->setEditable (false, false, false);
    unknown7->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown7->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown7->setBounds (352, 520, 80, 24);

    pitchSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (pitchSlider_.get());
    pitchSlider_->setRange (0, 10, 0);
    pitchSlider_->setSliderStyle (juce::Slider::LinearBar);
    pitchSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    pitchSlider_->addListener (this);

    pitchSlider_->setBounds (592, 544, 80, 24);

    unknown8.reset (new juce::Label (juce::String(),
                                     TRANS("Tune")));
    addAndMakeVisible (unknown8.get());
    unknown8->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown8->setJustificationType (juce::Justification::centred);
    unknown8->setEditable (false, false, false);
    unknown8->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown8->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown8->setBounds (568, 520, 104, 24);

    panSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (panSlider_.get());
    panSlider_->setRange (0, 10, 0);
    panSlider_->setSliderStyle (juce::Slider::LinearBar);
    panSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    panSlider_->addListener (this);

    panSlider_->setBounds (704, 544, 80, 24);

    unknown9.reset (new juce::Label (juce::String(),
                                     TRANS("Pan")));
    addAndMakeVisible (unknown9.get());
    unknown9->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown9->setJustificationType (juce::Justification::centred);
    unknown9->setEditable (false, false, false);
    unknown9->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown9->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown9->setBounds (680, 520, 104, 24);

    widthSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (widthSlider_.get());
    widthSlider_->setRange (0, 10, 0);
    widthSlider_->setSliderStyle (juce::Slider::LinearBar);
    widthSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    widthSlider_->addListener (this);

    widthSlider_->setBounds (792, 544, 80, 24);

    unknown10.reset (new juce::Label (juce::String(),
                                      TRANS("Width")));
    addAndMakeVisible (unknown10.get());
    unknown10->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown10->setJustificationType (juce::Justification::centred);
    unknown10->setEditable (false, false, false);
    unknown10->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown10->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown10->setBounds (792, 520, 80, 24);

    levelSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (levelSlider_.get());
    levelSlider_->setRange (0, 10, 0);
    levelSlider_->setSliderStyle (juce::Slider::LinearBar);
    levelSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    levelSlider_->addListener (this);

    levelSlider_->setBounds (904, 544, 80, 24);

    unknown11.reset (new juce::Label (juce::String(),
                                      TRANS("Level")));
    addAndMakeVisible (unknown11.get());
    unknown11->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown11->setJustificationType (juce::Justification::centred);
    unknown11->setEditable (false, false, false);
    unknown11->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown11->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown11->setBounds (880, 520, 104, 24);

    unknown12.reset (new juce::Label (juce::String(),
                                      TRANS("Dry")));
    addAndMakeVisible (unknown12.get());
    unknown12->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown12->setJustificationType (juce::Justification::centred);
    unknown12->setEditable (false, false, false);
    unknown12->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown12->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown12->setBounds (880, 272, 48, 24);

    unknown13.reset (new juce::Label (juce::String(),
                                      TRANS("Wet")));
    addAndMakeVisible (unknown13.get());
    unknown13->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown13->setJustificationType (juce::Justification::centred);
    unknown13->setEditable (false, false, false);
    unknown13->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown13->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown13->setBounds (936, 272, 48, 24);

    syncButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (syncButton_.get());
    syncButton_->setButtonText (TRANS("Sync"));
    syncButton_->addListener (this);

    syncButton_->setBounds (16, 56, 104, 56);

    gridChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (gridChoice_.get());
    gridChoice_->setEditableText (false);
    gridChoice_->setJustificationType (juce::Justification::centredLeft);
    gridChoice_->setTextWhenNothingSelected (juce::String());
    gridChoice_->setTextWhenNoChoicesAvailable (juce::String());
    gridChoice_->addListener (this);

    gridChoice_->setBounds (16, 152, 104, 24);

    unknown14.reset (new juce::Label (juce::String(),
                                      TRANS("Grid")));
    addAndMakeVisible (unknown14.get());
    unknown14->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown14->setJustificationType (juce::Justification::centred);
    unknown14->setEditable (false, false, false);
    unknown14->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown14->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown14->setBounds (16, 128, 104, 24);

    menuButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (menuButton_.get());
    menuButton_->addListener (this);

    menuButton_->setBounds (40, 8, 24, 24);

    unknown15.reset (new juce::Label (juce::String(),
                                      TRANS("Swing")));
    addAndMakeVisible (unknown15.get());
    unknown15->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown15->setJustificationType (juce::Justification::centred);
    unknown15->setEditable (false, false, false);
    unknown15->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown15->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown15->setBounds (16, 192, 104, 24);

    swingSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (swingSlider_.get());
    swingSlider_->setRange (0, 10, 0);
    swingSlider_->setSliderStyle (juce::Slider::LinearBar);
    swingSlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    swingSlider_->addListener (this);

    swingSlider_->setBounds (16, 216, 104, 24);

    filterEnableButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (filterEnableButton_.get());
    filterEnableButton_->setConnectedEdges (juce::Button::ConnectedOnRight);
    filterEnableButton_->addListener (this);

    filterEnableButton_->setBounds (16, 544, 24, 24);

    tuneEnableButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (tuneEnableButton_.get());
    tuneEnableButton_->setConnectedEdges (juce::Button::ConnectedOnRight);
    tuneEnableButton_->addListener (this);

    tuneEnableButton_->setBounds (568, 544, 24, 24);

    muteButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (muteButton_.get());
    muteButton_->setConnectedEdges (juce::Button::ConnectedOnRight);
    muteButton_->addListener (this);

    muteButton_->setBounds (880, 544, 24, 24);

    feedbackEnableButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (feedbackEnableButton_.get());
    feedbackEnableButton_->setConnectedEdges (juce::Button::ConnectedOnRight);
    feedbackEnableButton_->addListener (this);

    feedbackEnableButton_->setBounds (880, 88, 24, 24);

    flipEnableButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (flipEnableButton_.get());
    flipEnableButton_->setConnectedEdges (juce::Button::ConnectedOnRight);
    flipEnableButton_->addListener (this);

    flipEnableButton_->setBounds (680, 544, 24, 24);

    activeTapChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (activeTapChoice_.get());
    activeTapChoice_->setEditableText (false);
    activeTapChoice_->setJustificationType (juce::Justification::centred);
    activeTapChoice_->setTextWhenNothingSelected (juce::String());
    activeTapChoice_->setTextWhenNoChoicesAvailable (juce::String());
    activeTapChoice_->addListener (this);

    activeTapChoice_->setBounds (448, 440, 104, 32);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1000, 600);


    //[Constructor] You can add your own custom stuff here..
    impl.setEditMode(kTapEditLevel);

    tapEditScreen_->addListener(&impl);

    syncButton_->setClickingTogglesState(true);
    tapEnabledButton_->setClickingTogglesState(true);

    LookAndFeel::setTextButtonFont(*menuButton_, juce::Font("Fontaudio", 12.0f, juce::Font::plain));
    menuButton_->setButtonText(fontaudio::HExpand);

    juce::TextButton *powerButtons[] = {
        feedbackEnableButton_.get(),
        tapEnabledButton_.get(),
        filterEnableButton_.get(),
        tuneEnableButton_.get(),
        muteButton_.get(),
    };
    for (juce::TextButton *powerButton : powerButtons) {
        float textHeight = 12.0f;
        if (powerButton == tapEnabledButton_.get())
            textHeight = 24.0f;
        LookAndFeel::setTextButtonFont(*powerButton, juce::Font("Fontaudio", textHeight, juce::Font::plain));
        powerButton->setClickingTogglesState(true);
        powerButton->setButtonText(fontaudio::Powerswitch);
    }

    LookAndFeel::setTextButtonFont(*flipEnableButton_, juce::Font("Fontaudio", 12.0f, juce::Font::plain));
    flipEnableButton_->setClickingTogglesState(true);
    flipEnableButton_->setButtonText(fontaudio::Diskio);

    LookAndFeel::setComboBoxFont(*activeTapChoice_, juce::Font(24.0f));
    //[/Constructor]
}

MainComponent::~MainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    tapEditScreen_ = nullptr;
    cutoffButton_ = nullptr;
    resonanceButton_ = nullptr;
    tuneButton_ = nullptr;
    panButton_ = nullptr;
    levelButton_ = nullptr;
    firstTapButton_ = nullptr;
    lastTapButton_ = nullptr;
    tapDelaySlider_ = nullptr;
    unknown = nullptr;
    tapEnabledButton_ = nullptr;
    feedbackTapChoice_ = nullptr;
    unknown2 = nullptr;
    feedbackTapGainSlider_ = nullptr;
    unknown3 = nullptr;
    drySlider_ = nullptr;
    wetSlider_ = nullptr;
    filterChoice_ = nullptr;
    unknown4 = nullptr;
    hpfCutoffSlider_ = nullptr;
    lpfCutoffSlider_ = nullptr;
    unknown5 = nullptr;
    unknown6 = nullptr;
    resonanceSlider_ = nullptr;
    unknown7 = nullptr;
    pitchSlider_ = nullptr;
    unknown8 = nullptr;
    panSlider_ = nullptr;
    unknown9 = nullptr;
    widthSlider_ = nullptr;
    unknown10 = nullptr;
    levelSlider_ = nullptr;
    unknown11 = nullptr;
    unknown12 = nullptr;
    unknown13 = nullptr;
    syncButton_ = nullptr;
    gridChoice_ = nullptr;
    unknown14 = nullptr;
    menuButton_ = nullptr;
    unknown15 = nullptr;
    swingSlider_ = nullptr;
    filterEnableButton_ = nullptr;
    tuneEnableButton_ = nullptr;
    muteButton_ = nullptr;
    feedbackEnableButton_ = nullptr;
    flipEnableButton_ = nullptr;
    activeTapChoice_ = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    {
        float x = 136.0f, y = 40.0f, width = 728.0f, height = 384.0f;
        juce::Colour fillColour = juce::Colour (0xff2a7ca5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    {
        float x = 8.0f, y = 431.0f, width = 984.0f, height = 161.0f;
        juce::Colour fillColour = juce::Colour (0xffc86d4f);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    {
        float x = 872.0f, y = 40.0f, width = 120.0f, height = 192.0f;
        juce::Colour fillColour = juce::Colour (0xfffff080);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    {
        float x = 872.0f, y = 240.0f, width = 120.0f, height = 184.0f;
        juce::Colour fillColour = juce::Colour (0xfffff080);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    {
        float x = 8.0f, y = 263.0f, width = 120.0f, height = 161.0f;
        juce::Colour fillColour = juce::Colour (0xfffff080);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    {
        float x = 8.0f, y = 40.0f, width = 120.0f, height = 216.0f;
        juce::Colour fillColour = juce::Colour (0xfffff080);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    Impl &impl = *impl_;
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == cutoffButton_.get())
    {
        //[UserButtonCode_cutoffButton_] -- add your button handler code here..
        impl.setEditMode(kTapEditCutoff);
        //[/UserButtonCode_cutoffButton_]
    }
    else if (buttonThatWasClicked == resonanceButton_.get())
    {
        //[UserButtonCode_resonanceButton_] -- add your button handler code here..
        impl.setEditMode(kTapEditResonance);
        //[/UserButtonCode_resonanceButton_]
    }
    else if (buttonThatWasClicked == tuneButton_.get())
    {
        //[UserButtonCode_tuneButton_] -- add your button handler code here..
        impl.setEditMode(kTapEditTune);
        //[/UserButtonCode_tuneButton_]
    }
    else if (buttonThatWasClicked == panButton_.get())
    {
        //[UserButtonCode_panButton_] -- add your button handler code here..
        impl.setEditMode(kTapEditPan);
        //[/UserButtonCode_panButton_]
    }
    else if (buttonThatWasClicked == levelButton_.get())
    {
        //[UserButtonCode_levelButton_] -- add your button handler code here..
        impl.setEditMode(kTapEditLevel);
        //[/UserButtonCode_levelButton_]
    }
    else if (buttonThatWasClicked == firstTapButton_.get())
    {
        //[UserButtonCode_firstTapButton_] -- add your button handler code here..
        tapEditScreen_->beginTap();
        //[/UserButtonCode_firstTapButton_]
    }
    else if (buttonThatWasClicked == lastTapButton_.get())
    {
        //[UserButtonCode_lastTapButton_] -- add your button handler code here..
        tapEditScreen_->endTap();
        //[/UserButtonCode_lastTapButton_]
    }
    else if (buttonThatWasClicked == tapEnabledButton_.get())
    {
        //[UserButtonCode_tapEnabledButton_] -- add your button handler code here..
        //[/UserButtonCode_tapEnabledButton_]
    }
    else if (buttonThatWasClicked == syncButton_.get())
    {
        //[UserButtonCode_syncButton_] -- add your button handler code here..
        //[/UserButtonCode_syncButton_]
    }
    else if (buttonThatWasClicked == menuButton_.get())
    {
        //[UserButtonCode_menuButton_] -- add your button handler code here..
        //[/UserButtonCode_menuButton_]
    }
    else if (buttonThatWasClicked == filterEnableButton_.get())
    {
        //[UserButtonCode_filterEnableButton_] -- add your button handler code here..
        //[/UserButtonCode_filterEnableButton_]
    }
    else if (buttonThatWasClicked == tuneEnableButton_.get())
    {
        //[UserButtonCode_tuneEnableButton_] -- add your button handler code here..
        //[/UserButtonCode_tuneEnableButton_]
    }
    else if (buttonThatWasClicked == muteButton_.get())
    {
        //[UserButtonCode_muteButton_] -- add your button handler code here..
        //[/UserButtonCode_muteButton_]
    }
    else if (buttonThatWasClicked == feedbackEnableButton_.get())
    {
        //[UserButtonCode_feedbackEnableButton_] -- add your button handler code here..
        //[/UserButtonCode_feedbackEnableButton_]
    }
    else if (buttonThatWasClicked == flipEnableButton_.get())
    {
        //[UserButtonCode_flipEnableButton_] -- add your button handler code here..
        //[/UserButtonCode_flipEnableButton_]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MainComponent::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == tapDelaySlider_.get())
    {
        //[UserSliderCode_tapDelaySlider_] -- add your slider handling code here..
        //[/UserSliderCode_tapDelaySlider_]
    }
    else if (sliderThatWasMoved == feedbackTapGainSlider_.get())
    {
        //[UserSliderCode_feedbackTapGainSlider_] -- add your slider handling code here..
        //[/UserSliderCode_feedbackTapGainSlider_]
    }
    else if (sliderThatWasMoved == drySlider_.get())
    {
        //[UserSliderCode_drySlider_] -- add your slider handling code here..
        //[/UserSliderCode_drySlider_]
    }
    else if (sliderThatWasMoved == wetSlider_.get())
    {
        //[UserSliderCode_wetSlider_] -- add your slider handling code here..
        //[/UserSliderCode_wetSlider_]
    }
    else if (sliderThatWasMoved == hpfCutoffSlider_.get())
    {
        //[UserSliderCode_hpfCutoffSlider_] -- add your slider handling code here..
        //[/UserSliderCode_hpfCutoffSlider_]
    }
    else if (sliderThatWasMoved == lpfCutoffSlider_.get())
    {
        //[UserSliderCode_lpfCutoffSlider_] -- add your slider handling code here..
        //[/UserSliderCode_lpfCutoffSlider_]
    }
    else if (sliderThatWasMoved == resonanceSlider_.get())
    {
        //[UserSliderCode_resonanceSlider_] -- add your slider handling code here..
        //[/UserSliderCode_resonanceSlider_]
    }
    else if (sliderThatWasMoved == pitchSlider_.get())
    {
        //[UserSliderCode_pitchSlider_] -- add your slider handling code here..
        //[/UserSliderCode_pitchSlider_]
    }
    else if (sliderThatWasMoved == panSlider_.get())
    {
        //[UserSliderCode_panSlider_] -- add your slider handling code here..
        //[/UserSliderCode_panSlider_]
    }
    else if (sliderThatWasMoved == widthSlider_.get())
    {
        //[UserSliderCode_widthSlider_] -- add your slider handling code here..
        //[/UserSliderCode_widthSlider_]
    }
    else if (sliderThatWasMoved == levelSlider_.get())
    {
        //[UserSliderCode_levelSlider_] -- add your slider handling code here..
        //[/UserSliderCode_levelSlider_]
    }
    else if (sliderThatWasMoved == swingSlider_.get())
    {
        //[UserSliderCode_swingSlider_] -- add your slider handling code here..
        //[/UserSliderCode_swingSlider_]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void MainComponent::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == feedbackTapChoice_.get())
    {
        //[UserComboBoxCode_feedbackTapChoice_] -- add your combo box handling code here..
        //[/UserComboBoxCode_feedbackTapChoice_]
    }
    else if (comboBoxThatHasChanged == filterChoice_.get())
    {
        //[UserComboBoxCode_filterChoice_] -- add your combo box handling code here..
        //[/UserComboBoxCode_filterChoice_]
    }
    else if (comboBoxThatHasChanged == gridChoice_.get())
    {
        //[UserComboBoxCode_gridChoice_] -- add your combo box handling code here..
        //[/UserComboBoxCode_gridChoice_]
    }
    else if (comboBoxThatHasChanged == activeTapChoice_.get())
    {
        //[UserComboBoxCode_activeTapChoice_] -- add your combo box handling code here..
        //[/UserComboBoxCode_activeTapChoice_]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainComponent" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="1000" initialHeight="600">
  <BACKGROUND backgroundColour="ff323e44">
    <ROUNDRECT pos="136 40 728 384" cornerSize="5.0" fill="solid: ff2a7ca5"
               hasStroke="0"/>
    <ROUNDRECT pos="8 431 984 161" cornerSize="5.0" fill="solid: ffc86d4f" hasStroke="0"/>
    <ROUNDRECT pos="872 40 120 192" cornerSize="5.0" fill="solid: fffff080"
               hasStroke="0"/>
    <ROUNDRECT pos="872 240 120 184" cornerSize="5.0" fill="solid: fffff080"
               hasStroke="0"/>
    <ROUNDRECT pos="8 263 120 161" cornerSize="5.0" fill="solid: fffff080" hasStroke="0"/>
    <ROUNDRECT pos="8 40 120 216" cornerSize="5.0" fill="solid: fffff080" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="c36eda615afd52ad" memberName="tapEditScreen_" virtualName=""
                    explicitFocusOrder="0" pos="136 40 728 384" class="TapEditScreen"
                    params=""/>
  <TEXTBUTTON name="" id="fbe209bdcd7b5a8f" memberName="cutoffButton_" virtualName=""
              explicitFocusOrder="0" pos="160 8 136 24" buttonText="Cutoff"
              connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="b57b7360ebc749d9" memberName="resonanceButton_" virtualName=""
              explicitFocusOrder="0" pos="296 8 136 24" buttonText="Resonance"
              connectedEdges="3" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="53c3c11e2e3cc96f" memberName="tuneButton_" virtualName=""
              explicitFocusOrder="0" pos="432 8 136 24" buttonText="Tune" connectedEdges="3"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="c12936716811e246" memberName="panButton_" virtualName=""
              explicitFocusOrder="0" pos="568 8 136 24" buttonText="Pan" connectedEdges="3"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7558d040ff50b9ed" memberName="levelButton_" virtualName=""
              explicitFocusOrder="0" pos="704 8 136 24" buttonText="Level"
              connectedEdges="1" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="43aa27172b96c021" memberName="firstTapButton_" virtualName=""
              explicitFocusOrder="0" pos="16 280 104 56" buttonText="Start tap"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="66ad3e0b5c14c7ae" memberName="lastTapButton_" virtualName=""
              explicitFocusOrder="0" pos="16 352 104 56" buttonText="Last tap"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <SLIDER name="" id="9f8e11541428b98a" memberName="tapDelaySlider_" virtualName=""
          explicitFocusOrder="0" pos="448 544 104 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="c604cf3d3c3ee5b1" memberName="unknown" virtualName=""
         explicitFocusOrder="0" pos="448 520 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Delay" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="7240b06205c10e61" memberName="tapEnabledButton_"
              virtualName="" explicitFocusOrder="0" pos="472 480 56 32" buttonText=""
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="4376ef98cd0f798e" memberName="feedbackTapChoice_"
            virtualName="" explicitFocusOrder="0" pos="904 88 80 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems=""/>
  <LABEL name="" id="db4d1c75cd692557" memberName="unknown2" virtualName=""
         explicitFocusOrder="0" pos="880 48 102 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Feedback" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="20.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="62fe1bfedd2c32eb" memberName="feedbackTapGainSlider_"
          virtualName="" explicitFocusOrder="0" pos="880 120 104 96" min="0.0"
          max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="9d4616ada30ba75d" memberName="unknown3" virtualName=""
         explicitFocusOrder="0" pos="880 248 102 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="20.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="b2206b062a09ac07" memberName="drySlider_" virtualName=""
          explicitFocusOrder="0" pos="880 288 48 128" min="0.0" max="10.0"
          int="0.0" style="LinearVertical" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="" id="ee65e871ce784fe1" memberName="wetSlider_" virtualName=""
          explicitFocusOrder="0" pos="936 288 48 128" min="0.0" max="10.0"
          int="0.0" style="LinearVertical" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <COMBOBOX name="" id="a5bfb32dfcd0d098" memberName="filterChoice_" virtualName=""
            explicitFocusOrder="0" pos="40 544 128 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="906ee5da5ccfb0f6" memberName="unknown4" virtualName=""
         explicitFocusOrder="0" pos="16 520 152 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="918797f645a07437" memberName="hpfCutoffSlider_" virtualName=""
          explicitFocusOrder="0" pos="176 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="" id="184bee36214dd11a" memberName="lpfCutoffSlider_" virtualName=""
          explicitFocusOrder="0" pos="264 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="5f3ddd07c93c41a6" memberName="unknown5" virtualName=""
         explicitFocusOrder="0" pos="176 520 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="HPF cutoff" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="8aeb3c214b9a746b" memberName="unknown6" virtualName=""
         explicitFocusOrder="0" pos="264 520 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="LPF cutoff" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="ce01d3af838ea92c" memberName="resonanceSlider_" virtualName=""
          explicitFocusOrder="0" pos="352 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="59b052aceabca3e7" memberName="unknown7" virtualName=""
         explicitFocusOrder="0" pos="352 520 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Resonance" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="5affa2208e6503a" memberName="pitchSlider_" virtualName=""
          explicitFocusOrder="0" pos="592 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="16005f9095f804c0" memberName="unknown8" virtualName=""
         explicitFocusOrder="0" pos="568 520 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tune" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="9db05a1846c0a580" memberName="panSlider_" virtualName=""
          explicitFocusOrder="0" pos="704 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="f049bece2c57a0b0" memberName="unknown9" virtualName=""
         explicitFocusOrder="0" pos="680 520 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Pan" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="fa27f85db75faeb0" memberName="widthSlider_" virtualName=""
          explicitFocusOrder="0" pos="792 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="903c9f3b27211a0c" memberName="unknown10" virtualName=""
         explicitFocusOrder="0" pos="792 520 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Width" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="5e1a25de948a2f2f" memberName="levelSlider_" virtualName=""
          explicitFocusOrder="0" pos="904 544 80 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="11492731a8f7e406" memberName="unknown11" virtualName=""
         explicitFocusOrder="0" pos="880 520 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Level" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="8c06205230379f25" memberName="unknown12" virtualName=""
         explicitFocusOrder="0" pos="880 272 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Dry" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="63b65948700e239e" memberName="unknown13" virtualName=""
         explicitFocusOrder="0" pos="936 272 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Wet" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="7ac12be0bb27f229" memberName="syncButton_" virtualName=""
              explicitFocusOrder="0" pos="16 56 104 56" buttonText="Sync" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="16cd36a80d420093" memberName="gridChoice_" virtualName=""
            explicitFocusOrder="0" pos="16 152 104 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems=""/>
  <LABEL name="" id="ca6f4632ff8df183" memberName="unknown14" virtualName=""
         explicitFocusOrder="0" pos="16 128 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Grid" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="3bd4ee7bb83de442" memberName="menuButton_" virtualName=""
              explicitFocusOrder="0" pos="40 8 24 24" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="be77851fc14d8ba" memberName="unknown15" virtualName=""
         explicitFocusOrder="0" pos="16 192 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Swing" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="da0e42fa3de72498" memberName="swingSlider_" virtualName=""
          explicitFocusOrder="0" pos="16 216 104 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <TEXTBUTTON name="" id="7bcc36a07c5c42ff" memberName="filterEnableButton_"
              virtualName="" explicitFocusOrder="0" pos="16 544 24 24" buttonText=""
              connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7496b3b0f072ed02" memberName="tuneEnableButton_"
              virtualName="" explicitFocusOrder="0" pos="568 544 24 24" buttonText=""
              connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="2f55e1ddbe6ca060" memberName="muteButton_" virtualName=""
              explicitFocusOrder="0" pos="880 544 24 24" buttonText="" connectedEdges="2"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="e43adf889edc780a" memberName="feedbackEnableButton_"
              virtualName="" explicitFocusOrder="0" pos="880 88 24 24" buttonText=""
              connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="26aa605edc3b0e21" memberName="flipEnableButton_"
              virtualName="" explicitFocusOrder="0" pos="680 544 24 24" buttonText=""
              connectedEdges="2" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="61cd9161ffe3a708" memberName="activeTapChoice_" virtualName=""
            explicitFocusOrder="0" pos="448 440 104 32" editable="0" layout="36"
            items="" textWhenNonSelected="" textWhenNoItems=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
void MainComponent::Impl::setEditMode(TapEditMode editMode)
{
    if (editMode_ == editMode)
        return;

    MainComponent *self = self_;
    self->tapEditScreen_->setEditMode(editMode);
    self->cutoffButton_->setToggleState(editMode == kTapEditCutoff, juce::dontSendNotification);
    self->resonanceButton_->setToggleState(editMode == kTapEditResonance, juce::dontSendNotification);
    self->tuneButton_->setToggleState(editMode == kTapEditTune, juce::dontSendNotification);
    self->panButton_->setToggleState(editMode == kTapEditPan, juce::dontSendNotification);
    self->levelButton_->setToggleState(editMode == kTapEditLevel, juce::dontSendNotification);
}

void MainComponent::Impl::tappingHasStarted(TapEditScreen *)
{
    MainComponent *self = self_;
    self->firstTapButton_->setButtonText(TRANS("Next tap"));
}

void MainComponent::Impl::tappingHasEnded(TapEditScreen *)
{
    MainComponent *self = self_;
    self->firstTapButton_->setButtonText(TRANS("Start tap"));
}
//[/EndFile]

