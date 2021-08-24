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

#pragma once

//[Headers]     -- You can add your own extra header files here --
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
    void setActiveTapLabelText(const juce::String &newText);
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
    std::unique_ptr<juce::Label> timeRangeLabel_;
    std::unique_ptr<juce::TextButton> timeRangePlusButton_;
    std::unique_ptr<juce::TextButton> timeRangeMinusButton_;
    std::unique_ptr<juce::TextButton> firstTapButton_;
    std::unique_ptr<juce::TextButton> lastTapButton_;
    std::unique_ptr<juce::Label> activeTapLabel_;
    std::unique_ptr<juce::Slider> tapDelaySlider_;
    std::unique_ptr<juce::Label> unknown;
    std::unique_ptr<juce::TextButton> tapEnabledButton_;
    std::unique_ptr<juce::ComboBox> feedbackTapChoice_;
    std::unique_ptr<juce::Label> unknown2;
    std::unique_ptr<juce::Slider> feedbackTapGainSlider_;
    std::unique_ptr<juce::Label> unknown3;
    std::unique_ptr<juce::Slider> drySlider_;
    std::unique_ptr<juce::Slider> wetSlider_;
    std::unique_ptr<juce::ComboBox> filterChoice_;
    std::unique_ptr<juce::Label> unknown4;
    std::unique_ptr<juce::Slider> hpfCutoffSlider_;
    std::unique_ptr<juce::Slider> lpfCutoffSlider_;
    std::unique_ptr<juce::Label> unknown5;
    std::unique_ptr<juce::Label> unknown6;
    std::unique_ptr<juce::Slider> resonanceSlider_;
    std::unique_ptr<juce::Label> unknown7;
    std::unique_ptr<juce::Slider> pitchSlider_;
    std::unique_ptr<juce::Label> unknown8;
    std::unique_ptr<juce::Slider> panSlider_;
    std::unique_ptr<juce::Label> unknown9;
    std::unique_ptr<juce::Slider> widthSlider_;
    std::unique_ptr<juce::Label> unknown10;
    std::unique_ptr<juce::Slider> levelSlider_;
    std::unique_ptr<juce::Label> unknown11;
    std::unique_ptr<juce::Label> unknown12;
    std::unique_ptr<juce::Label> unknown13;
    std::unique_ptr<juce::ToggleButton> filterEnableButton_;
    std::unique_ptr<juce::ToggleButton> tuneEnableButton_;
    std::unique_ptr<juce::TextButton> syncButton_;
    std::unique_ptr<juce::ComboBox> gridChoice_;
    std::unique_ptr<juce::Label> unknown14;
    std::unique_ptr<juce::TextButton> menuButton_;
    std::unique_ptr<juce::ToggleButton> feedbackEnableButton_;
    std::unique_ptr<juce::Label> unknown15;
    std::unique_ptr<juce::Slider> swingSlider_;
    std::unique_ptr<juce::ToggleButton> muteButton_;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

