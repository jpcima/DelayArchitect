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
#include "editor/parts/FadGlyphButton.h"
#include "editor/parts/SVGGlyphButton.h"
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

    tapEditScreen_->setBounds (128, 72, 744, 348);

    cutoffButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (cutoffButton_.get());
    cutoffButton_->setButtonText (TRANS("Cutoff"));
    cutoffButton_->addListener (this);

    cutoffButton_->setBounds (144, 48, 136, 24);

    resonanceButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (resonanceButton_.get());
    resonanceButton_->setButtonText (TRANS("Resonance"));
    resonanceButton_->addListener (this);

    resonanceButton_->setBounds (288, 48, 136, 24);

    tuneButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (tuneButton_.get());
    tuneButton_->setButtonText (TRANS("Tune"));
    tuneButton_->addListener (this);

    tuneButton_->setBounds (432, 48, 136, 24);

    panButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (panButton_.get());
    panButton_->setButtonText (TRANS("Pan"));
    panButton_->addListener (this);

    panButton_->setBounds (576, 48, 136, 24);

    levelButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (levelButton_.get());
    levelButton_->setButtonText (TRANS("Level"));
    levelButton_->addListener (this);

    levelButton_->setBounds (720, 48, 136, 24);

    firstTapButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (firstTapButton_.get());
    firstTapButton_->setButtonText (TRANS("Start tap"));
    firstTapButton_->addListener (this);

    firstTapButton_->setBounds (16, 304, 96, 56);

    lastTapButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (lastTapButton_.get());
    lastTapButton_->setButtonText (TRANS("Last tap"));
    lastTapButton_->addListener (this);

    lastTapButton_->setBounds (16, 368, 96, 56);

    tapDelaySlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (tapDelaySlider_.get());
    tapDelaySlider_->setRange (0, 10, 0);
    tapDelaySlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    tapDelaySlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    tapDelaySlider_->addListener (this);

    tapDelaySlider_->setBounds (440, 520, 120, 88);

    unknown.reset (new juce::Label (juce::String(),
                                    TRANS("Delay")));
    addAndMakeVisible (unknown.get());
    unknown->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown->setJustificationType (juce::Justification::centred);
    unknown->setEditable (false, false, false);
    unknown->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown->setBounds (448, 504, 104, 24);

    feedbackTapChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (feedbackTapChoice_.get());
    feedbackTapChoice_->setEditableText (false);
    feedbackTapChoice_->setJustificationType (juce::Justification::centredLeft);
    feedbackTapChoice_->setTextWhenNothingSelected (juce::String());
    feedbackTapChoice_->setTextWhenNoChoicesAvailable (juce::String());
    feedbackTapChoice_->addListener (this);

    feedbackTapChoice_->setBounds (900, 80, 72, 24);

    unknown2.reset (new juce::Label (juce::String(),
                                     TRANS("Feedback")));
    addAndMakeVisible (unknown2.get());
    unknown2->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
    unknown2->setJustificationType (juce::Justification::centred);
    unknown2->setEditable (false, false, false);
    unknown2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown2->setBounds (908, 44, 80, 24);

    feedbackTapGainSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (feedbackTapGainSlider_.get());
    feedbackTapGainSlider_->setRange (0, 10, 0);
    feedbackTapGainSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackTapGainSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    feedbackTapGainSlider_->addListener (this);

    feedbackTapGainSlider_->setBounds (888, 104, 96, 88);

    unknown3.reset (new juce::Label (juce::String(),
                                     TRANS("Mix")));
    addAndMakeVisible (unknown3.get());
    unknown3->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
    unknown3->setJustificationType (juce::Justification::centred);
    unknown3->setEditable (false, false, false);
    unknown3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown3->setBounds (888, 212, 94, 24);

    drySlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (drySlider_.get());
    drySlider_->setRange (0, 10, 0);
    drySlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    drySlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    drySlider_->addListener (this);

    drySlider_->setBounds (888, 252, 96, 88);

    wetSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (wetSlider_.get());
    wetSlider_->setRange (0, 10, 0);
    wetSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    wetSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    wetSlider_->addListener (this);

    wetSlider_->setBounds (888, 360, 96, 88);

    filterChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (filterChoice_.get());
    filterChoice_->setEditableText (false);
    filterChoice_->setJustificationType (juce::Justification::centredLeft);
    filterChoice_->setTextWhenNothingSelected (juce::String());
    filterChoice_->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    filterChoice_->addListener (this);

    filterChoice_->setBounds (32, 540, 108, 24);

    unknown4.reset (new juce::Label (juce::String(),
                                     TRANS("Filter")));
    addAndMakeVisible (unknown4.get());
    unknown4->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown4->setJustificationType (juce::Justification::centred);
    unknown4->setEditable (false, false, false);
    unknown4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown4->setBounds (16, 504, 140, 24);

    hpfCutoffSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (hpfCutoffSlider_.get());
    hpfCutoffSlider_->setRange (0, 10, 0);
    hpfCutoffSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    hpfCutoffSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    hpfCutoffSlider_->addListener (this);

    hpfCutoffSlider_->setBounds (152, 520, 96, 88);

    lpfCutoffSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (lpfCutoffSlider_.get());
    lpfCutoffSlider_->setRange (0, 10, 0);
    lpfCutoffSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    lpfCutoffSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    lpfCutoffSlider_->addListener (this);

    lpfCutoffSlider_->setBounds (248, 520, 96, 88);

    unknown5.reset (new juce::Label (juce::String(),
                                     TRANS("HPF cutoff")));
    addAndMakeVisible (unknown5.get());
    unknown5->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown5->setJustificationType (juce::Justification::centred);
    unknown5->setEditable (false, false, false);
    unknown5->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown5->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown5->setBounds (160, 504, 80, 24);

    unknown6.reset (new juce::Label (juce::String(),
                                     TRANS("LPF cutoff")));
    addAndMakeVisible (unknown6.get());
    unknown6->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown6->setJustificationType (juce::Justification::centred);
    unknown6->setEditable (false, false, false);
    unknown6->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown6->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown6->setBounds (256, 504, 80, 24);

    resonanceSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (resonanceSlider_.get());
    resonanceSlider_->setRange (0, 10, 0);
    resonanceSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    resonanceSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    resonanceSlider_->addListener (this);

    resonanceSlider_->setBounds (344, 520, 96, 88);

    unknown7.reset (new juce::Label (juce::String(),
                                     TRANS("Resonance")));
    addAndMakeVisible (unknown7.get());
    unknown7->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown7->setJustificationType (juce::Justification::centred);
    unknown7->setEditable (false, false, false);
    unknown7->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown7->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown7->setBounds (352, 504, 80, 24);

    pitchSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (pitchSlider_.get());
    pitchSlider_->setRange (0, 10, 0);
    pitchSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    pitchSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    pitchSlider_->addListener (this);

    pitchSlider_->setBounds (564, 520, 96, 88);

    unknown8.reset (new juce::Label (juce::String(),
                                     TRANS("Tune")));
    addAndMakeVisible (unknown8.get());
    unknown8->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown8->setJustificationType (juce::Justification::centred);
    unknown8->setEditable (false, false, false);
    unknown8->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown8->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown8->setBounds (560, 504, 104, 24);

    panSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (panSlider_.get());
    panSlider_->setRange (0, 10, 0);
    panSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    panSlider_->addListener (this);

    panSlider_->setBounds (680, 520, 96, 88);

    unknown9.reset (new juce::Label (juce::String(),
                                     TRANS("Pan")));
    addAndMakeVisible (unknown9.get());
    unknown9->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown9->setJustificationType (juce::Justification::centred);
    unknown9->setEditable (false, false, false);
    unknown9->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown9->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown9->setBounds (676, 504, 104, 24);

    widthSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (widthSlider_.get());
    widthSlider_->setRange (0, 10, 0);
    widthSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    widthSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    widthSlider_->addListener (this);

    widthSlider_->setBounds (772, 520, 96, 88);

    unknown10.reset (new juce::Label (juce::String(),
                                      TRANS("Width")));
    addAndMakeVisible (unknown10.get());
    unknown10->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown10->setJustificationType (juce::Justification::centred);
    unknown10->setEditable (false, false, false);
    unknown10->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown10->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown10->setBounds (780, 504, 80, 24);

    levelSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (levelSlider_.get());
    levelSlider_->setRange (0, 10, 0);
    levelSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    levelSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    levelSlider_->addListener (this);

    levelSlider_->setBounds (884, 520, 96, 88);

    unknown11.reset (new juce::Label (juce::String(),
                                      TRANS("Level")));
    addAndMakeVisible (unknown11.get());
    unknown11->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown11->setJustificationType (juce::Justification::centred);
    unknown11->setEditable (false, false, false);
    unknown11->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown11->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown11->setBounds (880, 504, 104, 24);

    unknown12.reset (new juce::Label (juce::String(),
                                      TRANS("Dry")));
    addAndMakeVisible (unknown12.get());
    unknown12->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown12->setJustificationType (juce::Justification::centred);
    unknown12->setEditable (false, false, false);
    unknown12->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown12->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown12->setBounds (916, 236, 40, 24);

    unknown13.reset (new juce::Label (juce::String(),
                                      TRANS("Wet")));
    addAndMakeVisible (unknown13.get());
    unknown13->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown13->setJustificationType (juce::Justification::centred);
    unknown13->setEditable (false, false, false);
    unknown13->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown13->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown13->setBounds (916, 344, 40, 24);

    gridChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (gridChoice_.get());
    gridChoice_->setEditableText (false);
    gridChoice_->setJustificationType (juce::Justification::centredLeft);
    gridChoice_->setTextWhenNothingSelected (juce::String());
    gridChoice_->setTextWhenNoChoicesAvailable (juce::String());
    gridChoice_->addListener (this);

    gridChoice_->setBounds (28, 104, 72, 24);

    unknown14.reset (new juce::Label (juce::String(),
                                      TRANS("Grid")));
    addAndMakeVisible (unknown14.get());
    unknown14->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown14->setJustificationType (juce::Justification::centred);
    unknown14->setEditable (false, false, false);
    unknown14->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown14->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown14->setBounds (16, 80, 96, 24);

    unknown15.reset (new juce::Label (juce::String(),
                                      TRANS("Swing")));
    addAndMakeVisible (unknown15.get());
    unknown15->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown15->setJustificationType (juce::Justification::centred);
    unknown15->setEditable (false, false, false);
    unknown15->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown15->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown15->setBounds (16, 132, 96, 24);

    swingSlider_.reset (new better::Slider (juce::String()));
    addAndMakeVisible (swingSlider_.get());
    swingSlider_->setRange (0, 10, 0);
    swingSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    swingSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    swingSlider_->addListener (this);

    swingSlider_->setBounds (16, 148, 96, 88);

    activeTapChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (activeTapChoice_.get());
    activeTapChoice_->setEditableText (false);
    activeTapChoice_->setJustificationType (juce::Justification::centred);
    activeTapChoice_->setTextWhenNothingSelected (juce::String());
    activeTapChoice_->setTextWhenNoChoicesAvailable (juce::String());
    activeTapChoice_->addListener (this);

    activeTapChoice_->setBounds (448, 468, 104, 24);

    unknown16.reset (new juce::Label (juce::String(),
                                      TRANS("Sync")));
    addAndMakeVisible (unknown16.get());
    unknown16->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
    unknown16->setJustificationType (juce::Justification::centred);
    unknown16->setEditable (false, false, false);
    unknown16->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown16->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown16->setBounds (16, 44, 94, 24);

    unknown17.reset (new juce::Label (juce::String(),
                                      TRANS("Tap")));
    addAndMakeVisible (unknown17.get());
    unknown17->setFont (juce::Font (16.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
    unknown17->setJustificationType (juce::Justification::centred);
    unknown17->setEditable (false, false, false);
    unknown17->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown17->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown17->setBounds (16, 256, 94, 24);

    tapMiniMap_.reset (new TapMiniMap());
    addAndMakeVisible (tapMiniMap_.get());

    tapMiniMap_->setBounds (136, 420, 728, 32);

    logoButton_.reset (new juce::DrawableButton (juce::String{}, juce::DrawableButton::ImageFitted));
    addAndMakeVisible (logoButton_.get());

    logoButton_->setBounds (8, 4, 32, 32);

    patchNameEditor_.reset (new juce::TextEditor (juce::String()));
    addAndMakeVisible (patchNameEditor_.get());
    patchNameEditor_->setMultiLine (false);
    patchNameEditor_->setReturnKeyStartsNewLine (false);
    patchNameEditor_->setReadOnly (false);
    patchNameEditor_->setScrollbarsShown (true);
    patchNameEditor_->setCaretVisible (true);
    patchNameEditor_->setPopupMenuEnabled (true);
    patchNameEditor_->setText (juce::String());

    patchNameEditor_->setBounds (92, 8, 400, 24);

    filterEnableButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (filterEnableButton_.get());

    filterEnableButton_->setBounds (12, 500, 32, 32);

    tuneEnableButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (tuneEnableButton_.get());

    tuneEnableButton_->setBounds (556, 500, 32, 32);

    flipEnableButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (flipEnableButton_.get());

    flipEnableButton_->setBounds (676, 500, 32, 32);

    muteButton_.reset (new SVGGlyphButton (juce::String{}));
    addAndMakeVisible (muteButton_.get());

    muteButton_->setBounds (876, 500, 32, 32);

    feedbackEnableButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (feedbackEnableButton_.get());

    feedbackEnableButton_->setBounds (880, 40, 32, 32);

    syncButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (syncButton_.get());

    syncButton_->setBounds (8, 40, 32, 32);

    tapEnabledButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (tapEnabledButton_.get());

    tapEnabledButton_->setBounds (416, 464, 32, 32);

    tapMenuButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (tapMenuButton_.get());

    tapMenuButton_->setBounds (552, 464, 32, 32);

    menuButton_.reset (new FadGlyphButton (juce::String{}));
    addAndMakeVisible (menuButton_.get());

    menuButton_->setBounds (48, 4, 32, 32);

    filterAnalogChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (filterAnalogChoice_.get());
    filterAnalogChoice_->setEditableText (false);
    filterAnalogChoice_->setJustificationType (juce::Justification::centredLeft);
    filterAnalogChoice_->setTextWhenNothingSelected (juce::String());
    filterAnalogChoice_->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    filterAnalogChoice_->addListener (this);

    filterAnalogChoice_->setBounds (32, 568, 108, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1000, 620);


    //[Constructor] You can add your own custom stuff here..
    impl.setEditMode(kTapEditLevel);

    tapEditScreen_->addListener(&impl);

    syncButton_->setClickingTogglesState(true);
    tapEnabledButton_->setClickingTogglesState(true);

    feedbackEnableButton_->setClickingTogglesState(true);
    tapEnabledButton_->setClickingTogglesState(true);
    filterEnableButton_->setClickingTogglesState(true);
    tuneEnableButton_->setClickingTogglesState(true);
    muteButton_->setClickingTogglesState(true);
    flipEnableButton_->setClickingTogglesState(true);

    auto setupFadGlyphButton = [](FadGlyphButton &button, float textHeight, const fontaudio::IconName &iconName) {
        button.setIcon(iconName, textHeight);
    };
    setupFadGlyphButton(*menuButton_, 14.0f, fontaudio::HExpand);
    setupFadGlyphButton(*syncButton_, 14.0f, fontaudio::Powerswitch);
    setupFadGlyphButton(*feedbackEnableButton_, 14.0f, fontaudio::Powerswitch);
    setupFadGlyphButton(*tapEnabledButton_, 14.0f, fontaudio::Powerswitch);
    setupFadGlyphButton(*tapMenuButton_, 14.0f, fontaudio::HExpand);
    setupFadGlyphButton(*filterEnableButton_, 14.0f, fontaudio::Powerswitch);
    setupFadGlyphButton(*tuneEnableButton_, 14.0f, fontaudio::Powerswitch);
    setupFadGlyphButton(*flipEnableButton_, 14.0f, fontaudio::Diskio);

    {
        juce::String shapeOff = "m 112.2793,73.203125 a 6.0006,6.0006 0 0 0 -6.44532,0.996094 L 71.035156,105.22852 H 51.078125 a 6.0006,6.0006 0 0 0 -6,6 v 36.20703 a 6.0006,6.0006 0 0 0 6,6 h 20.101562 l 34.845703,28.54492 a 6.0006,6.0006 0 0 0 9.80078,-4.64063 V 78.677734 a 6.0006,6.0006 0 0 0 -3.54687,-5.474609 z m -8.45313,18.865234 V 164.66797 L 79.320312,144.59375 V 113.91797 Z M 57.078125,117.22852 h 10.242187 v 24.20703 H 57.078125 Z m 108.584115,-12.0586 a 6,6 0 0 0 -4.24414,1.75586 l -33.67969,33.67969 a 6,6 0 0 0 0,8.48633 6,6 0 0 0 8.48633,0 l 33.67968,-33.67969 a 6,6 0 0 0 0,-8.48633 6,6 0 0 0 -4.24218,-1.75586 z m -33.67969,0 a 6,6 0 0 0 -4.24414,1.75586 6,6 0 0 0 0,8.48633 l 33.67969,33.67969 a 6,6 0 0 0 8.48632,0 6,6 0 0 0 0,-8.48633 l -33.67968,-33.67969 a 6,6 0 0 0 -4.24219,-1.75586 z";
        juce::String shapeOn = "m 112.2793,73.203125 a 6.0006,6.0006 0 0 0 -6.44532,0.996094 L 71.035156,105.22852 H 51.078125 a 6.0006,6.0006 0 0 0 -6,6 v 36.20703 a 6.0006,6.0006 0 0 0 6,6 h 20.101562 l 34.845703,28.54492 a 6.0006,6.0006 0 0 0 9.80078,-4.64063 V 78.677734 a 6.0006,6.0006 0 0 0 -3.54687,-5.474609 z m -8.45313,18.865234 V 164.66797 L 79.320312,144.59375 V 113.91797 Z M 57.078125,117.22852 h 10.242187 v 24.20703 H 57.078125 Z M 150.47656,79.181641 a 6,6 0 0 0 -4.4082,1.287109 6,6 0 0 0 -0.91992,8.435547 c 18.37084,22.867633 18.37084,55.341343 0,78.208983 a 6,6 0 0 0 0.91992,8.43555 6,6 0 0 0 8.43555,-0.91992 c 21.85454,-27.20409 21.85454,-66.03615 0,-93.240238 a 6,6 0 0 0 -4.02735,-2.207031 z m -11.21875,11.498046 a 6,6 0 0 0 -4.45703,1.101563 6,6 0 0 0 -1.26953,8.39063 c 12.21586,16.57287 12.21586,39.10095 0,55.67382 a 6,6 0 0 0 1.26953,8.39063 6,6 0 0 0 8.38867,-1.26953 c 15.30687,-20.76635 15.30687,-49.14967 0,-69.916019 a 6,6 0 0 0 -3.93164,-2.371094 z m -12.20312,12.580083 a 6,6 0 0 0 -4.50977,0.85937 6,6 0 0 0 -1.72461,8.30859 c 6.21776,9.47315 6.21776,21.68896 0,31.16211 a 6,6 0 0 0 1.72461,8.3086 6,6 0 0 0 8.3086,-1.72461 c 8.82399,-13.44392 8.82399,-30.88616 0,-44.33008 a 6,6 0 0 0 -3.79883,-2.58398 z";
        muteButton_->setSVGPaths(shapeOff, shapeOn, 14.0f);
    }

    activeTapChoice_->setScrollWheelEnabled(true);
    feedbackTapChoice_->setScrollWheelEnabled(true);
    gridChoice_->setScrollWheelEnabled(true);
    filterChoice_->setScrollWheelEnabled(true);

    auto setEditButtonColors = [this](juce::TextButton &button, TapEditMode mode) {
        juce::LookAndFeel &lnf = getLookAndFeel();
        juce::Colour base = tapEditScreen_->getColourOfEditMode(lnf, mode);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour{});
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.125f));
        button.setColour(juce::TextButton::textColourOffId, base);
        button.setColour(juce::TextButton::textColourOnId, base.brighter());
        button.setColour(juce::ComboBox::outlineColourId, tapEditScreen_->findColour(TapEditScreen::lineColourId));
    };

    setEditButtonColors(*cutoffButton_, kTapEditCutoff);
    setEditButtonColors(*resonanceButton_, kTapEditResonance);
    setEditButtonColors(*tuneButton_, kTapEditTune);
    setEditButtonColors(*panButton_, kTapEditPan);
    setEditButtonColors(*levelButton_, kTapEditLevel);

    // stretch components which attach to the connected edge of a text button
    // by 1px towards the button, so it fuses into a single line
    //
    //juce::Component *leftExpandedComponents[] = {
    //};
    //juce::Component *rightExpandedComponents[] = {
    //};
    //for (juce::Component *comp : leftExpandedComponents) {
    //    juce::Rectangle<int> bounds = comp->getBounds();
    //    comp->setBounds(bounds.withLeft(bounds.getX() - 1));
    //}
    //for (juce::Component *comp : rightExpandedComponents) {
    //    juce::Rectangle<int> bounds = comp->getBounds();
    //    comp->setBounds(bounds.withRight(bounds.getRight() + 1));
    //}

    //
    juce::Path logoPath = juce::Drawable::parseSVGPath("M 397.05078 64.132812 C 378.80078 64.132812 359.79935 64.799479 346.66602 66.132812 C 292.79935 71.332812 243.46667 84.933854 202 105.86719 C 172.26667 121.06719 158.39987 131.06602 134.5332 154.66602 C 104.79987 184.26602 91.733073 205.46732 87.066406 231.33398 C 79.333073 275.20065 113.06576 322.40078 159.73242 332.80078 C 173.46576 335.86745 201.33372 334.80026 212.40039 330.93359 C 225.86706 326.00026 236.26732 319.59987 245.33398 310.5332 C 265.06732 290.66654 273.4668 266 269.4668 240 C 267.33346 226.66667 263.73398 219.20065 251.33398 203.33398 C 234.93398 182.13398 231.73255 172.53268 237.19922 160.66602 C 241.46589 151.33268 262.13411 135.86667 280.80078 128 C 298.53411 120.66667 312.53268 118.26641 334.66602 119.06641 C 357.33268 119.86641 365.06602 121.73372 382.66602 130.40039 C 410.53268 144.13372 422.66602 164.26719 426.66602 203.86719 C 427.46602 212.80052 428 295.2 428 438 C 428 678.53333 427.59961 665.99922 435.59961 691.19922 C 445.06628 721.33255 462.26732 740.93411 491.33398 754.80078 C 510.53398 763.86745 523.06615 766.66628 548.13281 767.59961 C 572.39948 768.53294 585.46589 766.93255 603.19922 761.19922 C 633.99922 751.06589 665.06641 730.00078 695.06641 698.80078 C 707.99974 685.33411 711.86667 678.53255 712 669.19922 C 712 662.53255 708.13255 657.33398 703.19922 657.33398 C 701.86589 657.33398 695.06758 661.20052 688.26758 665.86719 C 669.46758 678.80052 659.19922 681.46576 649.19922 675.73242 C 641.59922 671.46576 636.26641 662.39909 633.06641 647.73242 C 630.26641 635.59909 630.26732 631.73398 629.33398 443.33398 C 628.66732 328.13398 627.73359 244.4 626.93359 234 C 624.40026 203.46667 615.73333 178.13346 600 155.4668 C 591.6 143.33346 564.53385 116.80039 549.86719 106.40039 C 520.66719 85.600391 482.40026 71.199479 442.93359 66.132812 C 432.80026 64.799479 415.30078 64.132813 397.05078 64.132812 z M 247.61719 361.9043 C 240.9834 361.83516 234.4 362.12578 228 362.80078 C 184.53333 367.20078 146.80078 387.06706 118.80078 420.40039 C 82.800781 463.06706 64.933073 518.13333 67.066406 580 C 67.866406 603.2 69.733984 616.26602 75.333984 636.66602 C 89.867318 690.39935 124.26641 734.26667 167.06641 754 C 194.26641 766.53333 228.13424 770.93333 260.26758 766 C 307.46758 758.93333 352.40078 727.73242 378.80078 683.73242 C 386.26745 671.46576 387.06667 666.53255 382 663.19922 C 377.73333 660.39922 373.99935 661.73268 366.66602 668.66602 C 358.66602 676.26602 339.59961 690.13424 331.59961 694.26758 C 302.26628 709.06758 261.60065 703.59961 237.33398 681.59961 C 202.13398 649.86628 179.99909 588.53398 177.73242 517.33398 C 176.39909 474.53398 180.93346 446.79961 193.4668 421.59961 C 203.20013 401.59961 213.33359 392.93307 228.93359 391.06641 C 237.20026 390.13307 243.99948 392.26628 250.13281 397.59961 C 259.86615 406.13294 263.73255 416.66745 267.19922 444.80078 C 271.73255 479.46745 280.66576 495.06602 301.73242 504.66602 C 334.26576 519.59935 369.86615 502.53359 376.13281 468.93359 C 378.66615 455.06693 374.66576 436.4 365.73242 420 C 359.19909 408.26667 339.86667 389.33307 326 381.06641 C 306.06667 369.25807 276.36361 362.20391 247.61719 361.9043 z ");
    juce::DrawablePath logoDrawable;
    logoDrawable.setPath(logoPath);
    logoDrawable.setFill(juce::FillType(juce::Colours::white));
    logoButton_->setImages(&logoDrawable);

    //
    patchNameEditor_->setTextToShowWhenEmpty(TRANS("Untitled patch"), patchNameEditor_->findColour(juce::TextEditor::textColourId).darker());

    //
    tapEditScreen_->connectMiniMap(*tapMiniMap_);

    //[/Constructor]
}

MainComponent::~MainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    tapEditScreen_->disconnectMiniMap();
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
    gridChoice_ = nullptr;
    unknown14 = nullptr;
    unknown15 = nullptr;
    swingSlider_ = nullptr;
    activeTapChoice_ = nullptr;
    unknown16 = nullptr;
    unknown17 = nullptr;
    tapMiniMap_ = nullptr;
    logoButton_ = nullptr;
    patchNameEditor_ = nullptr;
    filterEnableButton_ = nullptr;
    tuneEnableButton_ = nullptr;
    flipEnableButton_ = nullptr;
    muteButton_ = nullptr;
    feedbackEnableButton_ = nullptr;
    syncButton_ = nullptr;
    tapEnabledButton_ = nullptr;
    tapMenuButton_ = nullptr;
    menuButton_ = nullptr;
    filterAnalogChoice_ = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff262626));

    {
        float x = 128.0f, y = 40.0f, width = 744.0f, height = 416.0f;
        juce::Colour fillColour = juce::Colour (0xff434343);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
    }

    {
        float x = 8.0f, y = 464.0f, width = 984.0f, height = 148.0f;
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 880.0f, y = 40.0f, width = 112.0f, height = 160.0f;
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 880.0f, y = 208.0f, width = 112.0f, height = 248.0f;
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 8.0f, y = 252.0f, width = 112.0f, height = 204.0f;
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 8.0f, y = 40.0f, width = 112.0f, height = 204.0f;
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 12.0f, y = 500.0f, width = 432.0f, height = 108.0f;
        juce::Colour fillColour = juce::Colour (0xff434343);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 556.0f, y = 500.0f, width = 112.0f, height = 108.0f;
        juce::Colour fillColour = juce::Colour (0xff434343);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 876.0f, y = 500.0f, width = 112.0f, height = 108.0f;
        juce::Colour fillColour = juce::Colour (0xff434343);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    {
        float x = 676.0f, y = 500.0f, width = 192.0f, height = 108.0f;
        juce::Colour fillColour = juce::Colour (0xff434343);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
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
    else if (comboBoxThatHasChanged == filterAnalogChoice_.get())
    {
        //[UserComboBoxCode_filterAnalogChoice_] -- add your combo box handling code here..
        //[/UserComboBoxCode_filterAnalogChoice_]
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
                 snapPixels="4" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="1000" initialHeight="620">
  <BACKGROUND backgroundColour="ff262626">
    <ROUNDRECT pos="128 40 744 416" cornerSize="5.0" fill="solid: ff434343"
               hasStroke="0"/>
    <ROUNDRECT pos="8 464 984 148" cornerSize="1.0" fill="solid: ff333333" hasStroke="0"/>
    <ROUNDRECT pos="880 40 112 160" cornerSize="1.0" fill="solid: ff333333"
               hasStroke="0"/>
    <ROUNDRECT pos="880 208 112 248" cornerSize="1.0" fill="solid: ff333333"
               hasStroke="0"/>
    <ROUNDRECT pos="8 252 112 204" cornerSize="1.0" fill="solid: ff333333" hasStroke="0"/>
    <ROUNDRECT pos="8 40 112 204" cornerSize="1.0" fill="solid: ff333333" hasStroke="0"/>
    <ROUNDRECT pos="12 500 432 108" cornerSize="1.0" fill="solid: ff434343"
               hasStroke="0"/>
    <ROUNDRECT pos="556 500 112 108" cornerSize="1.0" fill="solid: ff434343"
               hasStroke="0"/>
    <ROUNDRECT pos="876 500 112 108" cornerSize="1.0" fill="solid: ff434343"
               hasStroke="0"/>
    <ROUNDRECT pos="676 500 192 108" cornerSize="1.0" fill="solid: ff434343"
               hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="c36eda615afd52ad" memberName="tapEditScreen_" virtualName=""
                    explicitFocusOrder="0" pos="128 72 744 348" class="TapEditScreen"
                    params=""/>
  <TEXTBUTTON name="" id="fbe209bdcd7b5a8f" memberName="cutoffButton_" virtualName=""
              explicitFocusOrder="0" pos="144 48 136 24" buttonText="Cutoff"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="b57b7360ebc749d9" memberName="resonanceButton_" virtualName=""
              explicitFocusOrder="0" pos="288 48 136 24" buttonText="Resonance"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="53c3c11e2e3cc96f" memberName="tuneButton_" virtualName=""
              explicitFocusOrder="0" pos="432 48 136 24" buttonText="Tune"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="c12936716811e246" memberName="panButton_" virtualName=""
              explicitFocusOrder="0" pos="576 48 136 24" buttonText="Pan" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7558d040ff50b9ed" memberName="levelButton_" virtualName=""
              explicitFocusOrder="0" pos="720 48 136 24" buttonText="Level"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="43aa27172b96c021" memberName="firstTapButton_" virtualName=""
              explicitFocusOrder="0" pos="16 304 96 56" buttonText="Start tap"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="66ad3e0b5c14c7ae" memberName="lastTapButton_" virtualName=""
              explicitFocusOrder="0" pos="16 368 96 56" buttonText="Last tap"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <SLIDER name="" id="9f8e11541428b98a" memberName="tapDelaySlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="440 520 120 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="c604cf3d3c3ee5b1" memberName="unknown" virtualName=""
         explicitFocusOrder="0" pos="448 504 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Delay" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <COMBOBOX name="" id="4376ef98cd0f798e" memberName="feedbackTapChoice_"
            virtualName="" explicitFocusOrder="0" pos="900 80 72 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems=""/>
  <LABEL name="" id="db4d1c75cd692557" memberName="unknown2" virtualName=""
         explicitFocusOrder="0" pos="908 44 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Feedback" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
  <SLIDER name="" id="62fe1bfedd2c32eb" memberName="feedbackTapGainSlider_"
          virtualName="better::Slider" explicitFocusOrder="0" pos="888 104 96 88"
          min="0.0" max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="9d4616ada30ba75d" memberName="unknown3" virtualName=""
         explicitFocusOrder="0" pos="888 212 94 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
  <SLIDER name="" id="b2206b062a09ac07" memberName="drySlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="888 252 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="" id="ee65e871ce784fe1" memberName="wetSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="888 360 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <COMBOBOX name="" id="a5bfb32dfcd0d098" memberName="filterChoice_" virtualName=""
            explicitFocusOrder="0" pos="32 540 108 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="906ee5da5ccfb0f6" memberName="unknown4" virtualName=""
         explicitFocusOrder="0" pos="16 504 140 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="918797f645a07437" memberName="hpfCutoffSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="152 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="" id="184bee36214dd11a" memberName="lpfCutoffSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="248 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="5f3ddd07c93c41a6" memberName="unknown5" virtualName=""
         explicitFocusOrder="0" pos="160 504 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="HPF cutoff" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="8aeb3c214b9a746b" memberName="unknown6" virtualName=""
         explicitFocusOrder="0" pos="256 504 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="LPF cutoff" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="ce01d3af838ea92c" memberName="resonanceSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="344 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="59b052aceabca3e7" memberName="unknown7" virtualName=""
         explicitFocusOrder="0" pos="352 504 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Resonance" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="5affa2208e6503a" memberName="pitchSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="564 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="16005f9095f804c0" memberName="unknown8" virtualName=""
         explicitFocusOrder="0" pos="560 504 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tune" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="9db05a1846c0a580" memberName="panSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="680 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="f049bece2c57a0b0" memberName="unknown9" virtualName=""
         explicitFocusOrder="0" pos="676 504 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Pan" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="fa27f85db75faeb0" memberName="widthSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="772 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="903c9f3b27211a0c" memberName="unknown10" virtualName=""
         explicitFocusOrder="0" pos="780 504 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Width" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="5e1a25de948a2f2f" memberName="levelSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="884 520 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="11492731a8f7e406" memberName="unknown11" virtualName=""
         explicitFocusOrder="0" pos="880 504 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Level" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="8c06205230379f25" memberName="unknown12" virtualName=""
         explicitFocusOrder="0" pos="916 236 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Dry" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="63b65948700e239e" memberName="unknown13" virtualName=""
         explicitFocusOrder="0" pos="916 344 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Wet" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <COMBOBOX name="" id="16cd36a80d420093" memberName="gridChoice_" virtualName=""
            explicitFocusOrder="0" pos="28 104 72 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems=""/>
  <LABEL name="" id="ca6f4632ff8df183" memberName="unknown14" virtualName=""
         explicitFocusOrder="0" pos="16 80 96 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Grid" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="be77851fc14d8ba" memberName="unknown15" virtualName=""
         explicitFocusOrder="0" pos="16 132 96 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Swing" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="da0e42fa3de72498" memberName="swingSlider_" virtualName="better::Slider"
          explicitFocusOrder="0" pos="16 148 96 88" min="0.0" max="10.0"
          int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <COMBOBOX name="" id="61cd9161ffe3a708" memberName="activeTapChoice_" virtualName=""
            explicitFocusOrder="0" pos="448 468 104 24" editable="0" layout="36"
            items="" textWhenNonSelected="" textWhenNoItems=""/>
  <LABEL name="" id="25e83144e25df24d" memberName="unknown16" virtualName=""
         explicitFocusOrder="0" pos="16 44 94 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Sync" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
  <LABEL name="" id="a863161c796ae24c" memberName="unknown17" virtualName=""
         explicitFocusOrder="0" pos="16 256 94 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tap" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16.0"
         kerning="0.0" bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
  <GENERICCOMPONENT name="" id="d6577bae70b2754d" memberName="tapMiniMap_" virtualName=""
                    explicitFocusOrder="0" pos="136 420 728 32" class="TapMiniMap"
                    params=""/>
  <GENERICCOMPONENT name="" id="b9ee741e0a71c52b" memberName="logoButton_" virtualName=""
                    explicitFocusOrder="0" pos="8 4 32 32" class="juce::DrawableButton"
                    params="juce::String{}, juce::DrawableButton::ImageFitted"/>
  <TEXTEDITOR name="" id="f5168c8f248320c4" memberName="patchNameEditor_" virtualName=""
              explicitFocusOrder="0" pos="92 8 400 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <GENERICCOMPONENT name="" id="6f7446dd30f7a10a" memberName="filterEnableButton_"
                    virtualName="" explicitFocusOrder="0" pos="12 500 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="edc0aba9e3f1131e" memberName="tuneEnableButton_"
                    virtualName="" explicitFocusOrder="0" pos="556 500 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="6d041b00ec4a7429" memberName="flipEnableButton_"
                    virtualName="" explicitFocusOrder="0" pos="676 500 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="c22c2b3f78f64cdd" memberName="muteButton_" virtualName=""
                    explicitFocusOrder="0" pos="876 500 32 32" class="SVGGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="cca4aa87de0001d8" memberName="feedbackEnableButton_"
                    virtualName="" explicitFocusOrder="0" pos="880 40 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="fc0f07f2602c1b20" memberName="syncButton_" virtualName=""
                    explicitFocusOrder="0" pos="8 40 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="90adb3940d1f9143" memberName="tapEnabledButton_"
                    virtualName="" explicitFocusOrder="0" pos="416 464 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="bb2c4f96514254df" memberName="tapMenuButton_" virtualName=""
                    explicitFocusOrder="0" pos="552 464 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <GENERICCOMPONENT name="" id="b47f9836c0b72c69" memberName="menuButton_" virtualName=""
                    explicitFocusOrder="0" pos="48 4 32 32" class="FadGlyphButton"
                    params="juce::String{}"/>
  <COMBOBOX name="" id="cc686315cdb951b1" memberName="filterAnalogChoice_"
            virtualName="" explicitFocusOrder="0" pos="32 568 108 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
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

