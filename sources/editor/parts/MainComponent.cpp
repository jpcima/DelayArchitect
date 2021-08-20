/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.8

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "TapEditScreen.h"
#include <array>
#include <cstdio>
//[/Headers]

#include "MainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
struct MainComponent::Impl : public TapEditScreen::Listener {
    MainComponent *self_ = nullptr;
    TapEditMode editMode_ = kTapEditOff;
    int timeRangeIndex_ = -1;
    static constexpr std::array<float, 8> presetTimeRanges{{0.1f, 0.25f, 0.5f, 0.75f, 1.0f, 2.5f, 5.0f, 10.0f}};

    ///
    void setEditMode(TapEditMode editMode);
    void selectTimeRange(int index);

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

    tapEditScreen_->setBounds (144, 64, 712, 456);

    cutoffButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (cutoffButton_.get());
    cutoffButton_->setButtonText (TRANS("Cutoff"));
    cutoffButton_->addListener (this);

    cutoffButton_->setBounds (104, 32, 150, 24);

    resonanceButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (resonanceButton_.get());
    resonanceButton_->setButtonText (TRANS("Resonance"));
    resonanceButton_->addListener (this);

    resonanceButton_->setBounds (264, 32, 150, 24);

    tuneButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (tuneButton_.get());
    tuneButton_->setButtonText (TRANS("Tune"));
    tuneButton_->addListener (this);

    tuneButton_->setBounds (424, 32, 150, 24);

    panButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (panButton_.get());
    panButton_->setButtonText (TRANS("Pan"));
    panButton_->addListener (this);

    panButton_->setBounds (584, 32, 150, 24);

    levelButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (levelButton_.get());
    levelButton_->setButtonText (TRANS("Level"));
    levelButton_->addListener (this);

    levelButton_->setBounds (744, 32, 150, 24);

    timeRangeLabel_.reset (new juce::Label (juce::String(),
                                            TRANS("500 ms")));
    addAndMakeVisible (timeRangeLabel_.get());
    timeRangeLabel_->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    timeRangeLabel_->setJustificationType (juce::Justification::centredRight);
    timeRangeLabel_->setEditable (false, false, false);
    timeRangeLabel_->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    timeRangeLabel_->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    timeRangeLabel_->setBounds (832, 544, 80, 24);

    timeRangePlusButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (timeRangePlusButton_.get());
    timeRangePlusButton_->setButtonText (TRANS("+"));
    timeRangePlusButton_->addListener (this);

    timeRangePlusButton_->setBounds (944, 544, 22, 24);

    timeRangeMinusButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (timeRangeMinusButton_.get());
    timeRangeMinusButton_->setButtonText (TRANS("-"));
    timeRangeMinusButton_->addListener (this);

    timeRangeMinusButton_->setBounds (920, 544, 22, 24);

    firstTapButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (firstTapButton_.get());
    firstTapButton_->setButtonText (TRANS("Start tap"));
    firstTapButton_->addListener (this);

    firstTapButton_->setBounds (16, 304, 112, 56);

    lastTapButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (lastTapButton_.get());
    lastTapButton_->setButtonText (TRANS("Last tap"));
    lastTapButton_->addListener (this);

    lastTapButton_->setBounds (16, 376, 112, 56);

    activeTapLabel_.reset (new juce::Label (juce::String(),
                                            TRANS("Tap A")));
    addAndMakeVisible (activeTapLabel_.get());
    activeTapLabel_->setFont (juce::Font (30.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    activeTapLabel_->setJustificationType (juce::Justification::centred);
    activeTapLabel_->setEditable (false, false, false);
    activeTapLabel_->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    activeTapLabel_->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    activeTapLabel_->setBounds (424, 600, 150, 32);

    tapDelaySlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (tapDelaySlider_.get());
    tapDelaySlider_->setRange (0, 10, 0);
    tapDelaySlider_->setSliderStyle (juce::Slider::LinearBar);
    tapDelaySlider_->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    tapDelaySlider_->addListener (this);

    tapDelaySlider_->setBounds (424, 712, 150, 24);

    unknown.reset (new juce::Label (juce::String(),
                                    TRANS("Delay")));
    addAndMakeVisible (unknown.get());
    unknown->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown->setJustificationType (juce::Justification::centred);
    unknown->setEditable (false, false, false);
    unknown->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown->setBounds (424, 688, 150, 24);

    tapEnabledButton_.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (tapEnabledButton_.get());
    tapEnabledButton_->setButtonText (TRANS("Enabled"));
    tapEnabledButton_->addListener (this);

    tapEnabledButton_->setBounds (424, 648, 150, 24);

    feedbackTapChoice_.reset (new juce::ComboBox (juce::String()));
    addAndMakeVisible (feedbackTapChoice_.get());
    feedbackTapChoice_->setEditableText (false);
    feedbackTapChoice_->setJustificationType (juce::Justification::centredLeft);
    feedbackTapChoice_->setTextWhenNothingSelected (juce::String());
    feedbackTapChoice_->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    feedbackTapChoice_->addListener (this);

    feedbackTapChoice_->setBounds (872, 112, 112, 24);

    unknown2.reset (new juce::Label (juce::String(),
                                     TRANS("Feedback")));
    addAndMakeVisible (unknown2.get());
    unknown2->setFont (juce::Font (20.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown2->setJustificationType (juce::Justification::centred);
    unknown2->setEditable (false, false, false);
    unknown2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown2->setBounds (872, 72, 110, 24);

    feedbackTapGainSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (feedbackTapGainSlider_.get());
    feedbackTapGainSlider_->setRange (0, 10, 0);
    feedbackTapGainSlider_->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackTapGainSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    feedbackTapGainSlider_->addListener (this);

    feedbackTapGainSlider_->setBounds (872, 144, 112, 96);

    unknown3.reset (new juce::Label (juce::String(),
                                     TRANS("Mix")));
    addAndMakeVisible (unknown3.get());
    unknown3->setFont (juce::Font (20.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    unknown3->setJustificationType (juce::Justification::centred);
    unknown3->setEditable (false, false, false);
    unknown3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    unknown3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    unknown3->setBounds (872, 272, 110, 24);

    drySlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (drySlider_.get());
    drySlider_->setRange (0, 10, 0);
    drySlider_->setSliderStyle (juce::Slider::LinearVertical);
    drySlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    drySlider_->addListener (this);

    drySlider_->setBounds (872, 312, 48, 192);

    wetSlider_.reset (new juce::Slider (juce::String()));
    addAndMakeVisible (wetSlider_.get());
    wetSlider_->setRange (0, 10, 0);
    wetSlider_->setSliderStyle (juce::Slider::LinearVertical);
    wetSlider_->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    wetSlider_->addListener (this);

    wetSlider_->setBounds (936, 312, 48, 192);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (1000, 800);


    //[Constructor] You can add your own custom stuff here..
    impl.setEditMode(kTapEditLevel);
    impl.selectTimeRange((int)impl.presetTimeRanges.size() - 1);

    tapEditScreen_->addListener(&impl);

    tapEnabledButton_->setClickingTogglesState(true);
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
    timeRangeLabel_ = nullptr;
    timeRangePlusButton_ = nullptr;
    timeRangeMinusButton_ = nullptr;
    firstTapButton_ = nullptr;
    lastTapButton_ = nullptr;
    activeTapLabel_ = nullptr;
    tapDelaySlider_ = nullptr;
    unknown = nullptr;
    tapEnabledButton_ = nullptr;
    feedbackTapChoice_ = nullptr;
    unknown2 = nullptr;
    feedbackTapGainSlider_ = nullptr;
    unknown3 = nullptr;
    drySlider_ = nullptr;
    wetSlider_ = nullptr;


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
        float x = 144.0f, y = 64.0f, width = 712.0f, height = 456.0f;
        juce::Colour fillColour = juce::Colour (0xff2a7ca5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = 16.0f, y = 536.0f, width = 968.0f, height = 40.0f;
        juce::Colour fillColour = juce::Colour (0xff4fc892);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = 16.0f, y = 592.0f, width = 968.0f, height = 192.0f;
        juce::Colour fillColour = juce::Colour (0xffc86d4f);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = 864.0f, y = 64.0f, width = 128.0f, height = 192.0f;
        juce::Colour fillColour = juce::Colour (0xfffff080);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = 864.0f, y = 264.0f, width = 128.0f, height = 256.0f;
        juce::Colour fillColour = juce::Colour (0xfffff080);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
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
    else if (buttonThatWasClicked == timeRangePlusButton_.get())
    {
        //[UserButtonCode_timeRangePlusButton_] -- add your button handler code here..
        impl.selectTimeRange(impl.timeRangeIndex_ + 1);
        //[/UserButtonCode_timeRangePlusButton_]
    }
    else if (buttonThatWasClicked == timeRangeMinusButton_.get())
    {
        //[UserButtonCode_timeRangeMinusButton_] -- add your button handler code here..
        impl.selectTimeRange(impl.timeRangeIndex_ - 1);
        //[/UserButtonCode_timeRangeMinusButton_]
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
                 fixedSize="1" initialWidth="1000" initialHeight="800">
  <BACKGROUND backgroundColour="ff323e44">
    <ROUNDRECT pos="144 64 712 456" cornerSize="10.0" fill="solid: ff2a7ca5"
               hasStroke="0"/>
    <ROUNDRECT pos="16 536 968 40" cornerSize="10.0" fill="solid: ff4fc892"
               hasStroke="0"/>
    <ROUNDRECT pos="16 592 968 192" cornerSize="10.0" fill="solid: ffc86d4f"
               hasStroke="0"/>
    <ROUNDRECT pos="864 64 128 192" cornerSize="10.0" fill="solid: fffff080"
               hasStroke="0"/>
    <ROUNDRECT pos="864 264 128 256" cornerSize="10.0" fill="solid: fffff080"
               hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="c36eda615afd52ad" memberName="tapEditScreen_" virtualName=""
                    explicitFocusOrder="0" pos="144 64 712 456" class="TapEditScreen"
                    params=""/>
  <TEXTBUTTON name="" id="fbe209bdcd7b5a8f" memberName="cutoffButton_" virtualName=""
              explicitFocusOrder="0" pos="104 32 150 24" buttonText="Cutoff"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="b57b7360ebc749d9" memberName="resonanceButton_" virtualName=""
              explicitFocusOrder="0" pos="264 32 150 24" buttonText="Resonance"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="53c3c11e2e3cc96f" memberName="tuneButton_" virtualName=""
              explicitFocusOrder="0" pos="424 32 150 24" buttonText="Tune"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="c12936716811e246" memberName="panButton_" virtualName=""
              explicitFocusOrder="0" pos="584 32 150 24" buttonText="Pan" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7558d040ff50b9ed" memberName="levelButton_" virtualName=""
              explicitFocusOrder="0" pos="744 32 150 24" buttonText="Level"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="cafcacc6447ce988" memberName="timeRangeLabel_" virtualName=""
         explicitFocusOrder="0" pos="832 544 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="500 ms" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <TEXTBUTTON name="" id="5f270ebb614cc322" memberName="timeRangePlusButton_"
              virtualName="" explicitFocusOrder="0" pos="944 544 22 24" buttonText="+"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="194ef40c396e7316" memberName="timeRangeMinusButton_"
              virtualName="" explicitFocusOrder="0" pos="920 544 22 24" buttonText="-"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="43aa27172b96c021" memberName="firstTapButton_" virtualName=""
              explicitFocusOrder="0" pos="16 304 112 56" buttonText="Start tap"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="66ad3e0b5c14c7ae" memberName="lastTapButton_" virtualName=""
              explicitFocusOrder="0" pos="16 376 112 56" buttonText="Last tap"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="f36e1a9b08999a8f" memberName="activeTapLabel_" virtualName=""
         explicitFocusOrder="0" pos="424 600 150 32" edTextCol="ff000000"
         edBkgCol="0" labelText="Tap A" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="30.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="9f8e11541428b98a" memberName="tapDelaySlider_" virtualName=""
          explicitFocusOrder="0" pos="424 712 150 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="" id="c604cf3d3c3ee5b1" memberName="unknown" virtualName=""
         explicitFocusOrder="0" pos="424 688 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Delay" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="7240b06205c10e61" memberName="tapEnabledButton_"
              virtualName="" explicitFocusOrder="0" pos="424 648 150 24" buttonText="Enabled"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="4376ef98cd0f798e" memberName="feedbackTapChoice_"
            virtualName="" explicitFocusOrder="0" pos="872 112 112 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="db4d1c75cd692557" memberName="unknown2" virtualName=""
         explicitFocusOrder="0" pos="872 72 110 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Feedback" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="20.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="62fe1bfedd2c32eb" memberName="feedbackTapGainSlider_"
          virtualName="" explicitFocusOrder="0" pos="872 144 112 96" min="0.0"
          max="10.0" int="0.0" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="" id="9d4616ada30ba75d" memberName="unknown3" virtualName=""
         explicitFocusOrder="0" pos="872 272 110 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="20.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="" id="b2206b062a09ac07" memberName="drySlider_" virtualName=""
          explicitFocusOrder="0" pos="872 312 48 192" min="0.0" max="10.0"
          int="0.0" style="LinearVertical" textBoxPos="TextBoxBelow" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="" id="ee65e871ce784fe1" memberName="wetSlider_" virtualName=""
          explicitFocusOrder="0" pos="936 312 48 192" min="0.0" max="10.0"
          int="0.0" style="LinearVertical" textBoxPos="TextBoxBelow" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
void MainComponent::setActiveTapLabelText(const juce::String &newText)
{
    activeTapLabel_->setText(newText, juce::dontSendNotification);
}

constexpr std::array<float, 8> MainComponent::Impl::presetTimeRanges;

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

void MainComponent::Impl::selectTimeRange(int index)
{
    index = juce::jlimit(0, (int)presetTimeRanges.size() - 1, index);
    timeRangeIndex_ = index;
    float timeValue = presetTimeRanges[(size_t)index];

    MainComponent *self = self_;
    self->tapEditScreen_->setTimeRange(timeValue);

    char labelText[256];
    if (timeValue >= 1) {
        std::sprintf(labelText, "%g s", timeValue);
    }
    else {
        float ms = timeValue * 1000;
        std::sprintf(labelText, "%g ms", ms);
    }
    self->timeRangeLabel_->setText(labelText, juce::dontSendNotification);
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

