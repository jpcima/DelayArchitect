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
//[/Headers]

#include "AboutComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AboutComponent::AboutComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..

    // NOTE(jpc) cf. https://github.com/juce-framework/JUCE/issues/944
    using URL = juce::URL;

    //[/Constructor_pre]

    juce__label.reset (new juce::Label (juce::String(),
                                        TRANS("Delay Architect")));
    addAndMakeVisible (juce__label.get());
    juce__label->setFont (juce::Font (32.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label->setJustificationType (juce::Justification::centredLeft);
    juce__label->setEditable (false, false, false);
    juce__label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label->setBounds (16, 16, 216, 32);

    juce__label2.reset (new juce::Label (juce::String(),
                                         TRANS("A visual, musical editor for delay effects\n"
                                         "Create sophisticated delays, with simplicity.")));
    addAndMakeVisible (juce__label2.get());
    juce__label2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label2->setJustificationType (juce::Justification::centredLeft);
    juce__label2->setEditable (false, false, false);
    juce__label2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label2->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label2->setBounds (16, 64, 568, 56);

    juce__label3.reset (new juce::Label ("new label",
                                         TRANS("This program is published as free software.\n"
                                         "Contributions are welcome, in the form of ideas, code, or presets.")));
    addAndMakeVisible (juce__label3.get());
    juce__label3->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label3->setJustificationType (juce::Justification::centredLeft);
    juce__label3->setEditable (false, false, false);
    juce__label3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label3->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label3->setBounds (16, 128, 568, 56);

    juce__hyperlinkButton.reset (new juce::HyperlinkButton (TRANS("Report a problem"),
                                                            URL ("https://github.com/jpcima/DelayArchitect/issues")));
    addAndMakeVisible (juce__hyperlinkButton.get());
    juce__hyperlinkButton->setTooltip (TRANS("https://github.com/jpcima/DelayArchitect/issues"));
    juce__hyperlinkButton->setButtonText (TRANS("Report a problem"));

    juce__hyperlinkButton->setBounds (224, 256, 150, 24);

    juce__hyperlinkButton2.reset (new juce::HyperlinkButton (TRANS("Home page"),
                                                             URL ("https://github.com/jpcima/DelayArchitect")));
    addAndMakeVisible (juce__hyperlinkButton2.get());
    juce__hyperlinkButton2->setTooltip (TRANS("https://github.com/jpcima/DelayArchitect"));
    juce__hyperlinkButton2->setButtonText (TRANS("Home page"));

    juce__hyperlinkButton2->setBounds (224, 232, 150, 24);

    juce__label4.reset (new juce::Label ("new label",
                                         juce::CharPointer_UTF8 ("\xc2\xa9 Jean Pierre Cimalando 2021, GPL version 3.0 or later")));
    addAndMakeVisible (juce__label4.get());
    juce__label4->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label4->setJustificationType (juce::Justification::centredLeft);
    juce__label4->setEditable (false, false, false);
    juce__label4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label4->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label4->setBounds (16, 192, 568, 32);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 300);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AboutComponent::~AboutComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    juce__label = nullptr;
    juce__label2 = nullptr;
    juce__label3 = nullptr;
    juce__hyperlinkButton = nullptr;
    juce__hyperlinkButton2 = nullptr;
    juce__label4 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AboutComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xffefefef));

    {
        int x = 16, y = 64, width = 568, height = 1;
        juce::Colour fillColour = juce::Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AboutComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AboutComponent" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="300">
  <BACKGROUND backgroundColour="ffefefef">
    <RECT pos="16 64 568 1" fill="solid: ff000000" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="10bfc22b44c92168" memberName="juce__label" virtualName=""
         explicitFocusOrder="0" pos="16 16 216 32" edTextCol="ff000000"
         edBkgCol="0" labelText="Delay Architect" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="32.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="c7a56eb2a94a33c3" memberName="juce__label2" virtualName=""
         explicitFocusOrder="0" pos="16 64 568 56" edTextCol="ff000000"
         edBkgCol="0" labelText="A visual, musical editor for delay effects&#10;Create sophisticated delays, with simplicity."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <LABEL name="new label" id="397ef39e7be5ac3b" memberName="juce__label3"
         virtualName="" explicitFocusOrder="0" pos="16 128 568 56" edTextCol="ff000000"
         edBkgCol="0" labelText="This program is published as free software.&#10;Contributions are welcome, in the form of ideas, code, or presets."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <HYPERLINKBUTTON name="" id="89582401f2d99d7f" memberName="juce__hyperlinkButton"
                   virtualName="" explicitFocusOrder="0" pos="224 256 150 24" tooltip="https://github.com/jpcima/DelayArchitect/issues"
                   buttonText="Report a problem" connectedEdges="0" needsCallback="0"
                   radioGroupId="0" url="https://github.com/jpcima/DelayArchitect/issues"/>
  <HYPERLINKBUTTON name="" id="9a7aad47d78ec9c6" memberName="juce__hyperlinkButton2"
                   virtualName="" explicitFocusOrder="0" pos="224 232 150 24" tooltip="https://github.com/jpcima/DelayArchitect"
                   buttonText="Home page" connectedEdges="0" needsCallback="0" radioGroupId="0"
                   url="https://github.com/jpcima/DelayArchitect"/>
  <LABEL name="new label" id="965f6774d0d91c74" memberName="juce__label4"
         virtualName="" explicitFocusOrder="0" pos="16 192 568 32" edTextCol="ff000000"
         edBkgCol="0" labelText="&#169; Jean Pierre Cimalando 2021, GPL version 3.0 or later"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

