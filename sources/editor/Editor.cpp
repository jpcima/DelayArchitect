// SPDX-License-Identifier: BSD-2-Clause
#include "editor/Editor.h"
#include "editor/LookAndFeel.h"
#include "editor/parts/MainComponent.h"
#include "editor/parts/TapEditScreen.h"
#include "processor/Processor.h"
#include "GdDefs.h"

//==============================================================================
struct Editor::Impl : public TapEditScreen::Listener,
                      public juce::Slider::Listener,
                      public juce::Button::Listener,
                      public juce::ComboBox::Listener,
                      public juce::AudioProcessorParameter::Listener {
    Editor *self_ = nullptr;
    std::unique_ptr<MainComponent> mainComponent_;
    juce::Array<juce::AudioProcessorParameter *> parameters_;
    int activeTapNumber_ = 0;

    void setActiveTap(int tapNumber);
    void syncActiveTapParametersToControls();
    void syncActiveTapParameterToControls(int switchableIndex);
    void setSliderRangeFromParameter(juce::Slider *slider, int parameterIndex);
    void setComboBoxChoicesFromParameter(juce::ComboBox *comboBox, int parameterIndex);
    int getParameterForSlider(juce::Slider *slider);
    int getParameterForButton(juce::Button *button);
    int getParameterForComboBox(juce::ComboBox *comboBox);

    static void breakDownParameterIndex(int parameterIndex, int &switchableIndex, int &tapNumber);
    static int recomposeParameterIndex(int switchableIndex, int tapNumber);

    void tapEditStarted(TapEditScreen *, int tapNumber, ChangeId id) override;
    void tapEditEnded(TapEditScreen *, int tapNumber, ChangeId id) override;
    void tapValueChanged(TapEditScreen *, int tapNumber, ChangeId id, float value) override;

    void sliderDragStarted(juce::Slider *slider) override;
    void sliderDragEnded(juce::Slider *slider) override;
    void sliderValueChanged(juce::Slider *slider) override;

    void buttonClicked(juce::Button *button) override;
    void buttonStateChanged(juce::Button *button) override;

    void comboBoxChanged(juce::ComboBox *comboBox) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
};

//==============================================================================
Editor::Editor(Processor &p)
    : AudioProcessorEditor(p),
      impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    impl.parameters_ = p.getParameters();

    static LookAndFeel lnf;
    setLookAndFeel(&lnf);
    juce::LookAndFeel::setDefaultLookAndFeel(&lnf);

    MainComponent *mainComponent = new MainComponent;
    impl.mainComponent_.reset(mainComponent);

    setSize(mainComponent->getWidth(), mainComponent->getHeight());
    addAndMakeVisible(mainComponent);

    impl.setSliderRangeFromParameter(mainComponent->getTapDelaySlider(), GDP_TAP_A_DELAY);
    impl.setComboBoxChoicesFromParameter(mainComponent->getFeedbackTapChoice(), GDP_FEEDBACK_TAP);
    impl.setSliderRangeFromParameter(mainComponent->getFeedbackTapGainSlider(), GDP_FEEDBACK_GAIN);
    impl.setSliderRangeFromParameter(mainComponent->getWetSlider(), GDP_MIX_WET);
    impl.setSliderRangeFromParameter(mainComponent->getDrySlider(), GDP_MIX_DRY);

    mainComponent->getFeedbackTapGainSlider()->setNumDecimalPlacesToDisplay(2);
    mainComponent->getWetSlider()->setNumDecimalPlacesToDisplay(2);
    mainComponent->getDrySlider()->setNumDecimalPlacesToDisplay(2);

    for (int i = 0, n = impl.parameters_.size(); i < n; ++i) {
        juce::AudioProcessorParameter *parameter = impl.parameters_[i];
        parameter->addListener(&impl);
        impl.parameterValueChanged(i, parameter->getValue());
    }

    TapEditScreen *tapEdit = mainComponent->getTapEditScreen();
    tapEdit->addListener(&impl);

    mainComponent->getTapEnabledButton()->addListener(&impl);
    mainComponent->getTapDelaySlider()->addListener(&impl);
    mainComponent->getFeedbackTapChoice()->addListener(&impl);
    mainComponent->getFeedbackTapGainSlider()->addListener(&impl);
    mainComponent->getWetSlider()->addListener(&impl);
    mainComponent->getDrySlider()->addListener(&impl);

    impl.syncActiveTapParametersToControls();
}

Editor::~Editor()
{
    Impl &impl = *impl_;
    MainComponent &mainComponent = *impl.mainComponent_;

    mainComponent.getTapEnabledButton()->removeListener(&impl);
    mainComponent.getTapDelaySlider()->removeListener(&impl);
    mainComponent.getFeedbackTapChoice()->removeListener(&impl);

    for (int i = 0, n = impl.parameters_.size(); i < n; ++i) {
        juce::AudioProcessorParameter *parameter = impl.parameters_[i];
        parameter->removeListener(&impl);
    }
}

void Editor::Impl::setActiveTap(int tapNumber)
{
    if (activeTapNumber_ == tapNumber)
        return;

    activeTapNumber_ = tapNumber;
    syncActiveTapParametersToControls();
}

void Editor::Impl::syncActiveTapParametersToControls()
{
    MainComponent &mainComponent = *mainComponent_;
    int tapNumber = activeTapNumber_;
    char tapChar = (char)('A' + tapNumber);

    juce::String tapFormat = TRANS("Tap %c");
    char tapText[256];
    std::sprintf(tapText, tapFormat.toRawUTF8(), tapChar);
    mainComponent.setActiveTapLabelText(tapText);

    ///
    for (int i = 0; i < GDP_TAP_B_ENABLE; ++i)
        syncActiveTapParameterToControls(i);
}

void Editor::Impl::syncActiveTapParameterToControls(int switchableIndex)
{
    int tapNumber = activeTapNumber_;
    int parameterIndex = recomposeParameterIndex(switchableIndex, tapNumber);
    juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);

    float value = parameter->convertFrom0to1(parameter->getValue());

    switch (switchableIndex) {
    case GDP_TAP_A_ENABLE:
        mainComponent_->getTapEnabledButton()->setToggleState((bool)value, juce::dontSendNotification);
        break;
    case GDP_TAP_A_DELAY:
        mainComponent_->getTapDelaySlider()->setValue(value, juce::dontSendNotification);
        break;
    }
}

void Editor::Impl::setSliderRangeFromParameter(juce::Slider *slider, int parameterIndex)
{
    juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);
    slider->setRange(parameter->getNormalisableRange().start, parameter->getNormalisableRange().end);
}

void Editor::Impl::setComboBoxChoicesFromParameter(juce::ComboBox *comboBox, int parameterIndex)
{
    juce::AudioParameterChoice *parameter = static_cast<juce::AudioParameterChoice *>(parameters_[parameterIndex]);

    int numChoices = parameter->choices.size();
    for (int i = 0; i < numChoices; ++i) {
        const juce::String &choice = parameter->choices[i];
        comboBox->addItem(choice, i + 1);
    }
}

int Editor::Impl::getParameterForSlider(juce::Slider *slider)
{
    MainComponent &mainComponent = *mainComponent_;
    int tapNumber = activeTapNumber_;
    int switchableIndex = -1;

    if (slider == mainComponent.getTapDelaySlider())
        switchableIndex = GDP_TAP_A_DELAY;
    else if (slider == mainComponent.getFeedbackTapGainSlider())
        switchableIndex = GDP_FEEDBACK_GAIN;
    else if (slider == mainComponent.getWetSlider())
        switchableIndex = GDP_MIX_WET;
    else if (slider == mainComponent.getDrySlider())
        switchableIndex = GDP_MIX_DRY;

    if (switchableIndex == -1)
        return -1;
    else
        return recomposeParameterIndex(switchableIndex, tapNumber);
}

int Editor::Impl::getParameterForButton(juce::Button *button)
{
    MainComponent &mainComponent = *mainComponent_;
    int tapNumber = activeTapNumber_;
    int switchableIndex = -1;

    if (button == mainComponent.getTapEnabledButton())
        switchableIndex = GDP_TAP_A_ENABLE;

    if (switchableIndex == -1)
        return -1;
    else
        return recomposeParameterIndex(switchableIndex, tapNumber);
}

int Editor::Impl::getParameterForComboBox(juce::ComboBox *comboBox)
{
    MainComponent &mainComponent = *mainComponent_;
    int tapNumber = activeTapNumber_;
    int switchableIndex = -1;

    if (comboBox == mainComponent.getFeedbackTapChoice())
        switchableIndex = GDP_FEEDBACK_TAP;

    if (switchableIndex == -1)
        return -1;
    else
        return recomposeParameterIndex(switchableIndex, tapNumber);
}

void Editor::Impl::breakDownParameterIndex(int parameterIndex, int &switchableIndex, int &tapNumber)
{
    if (parameterIndex < GDP_TAP_A_ENABLE) {
        switchableIndex = parameterIndex;
        tapNumber = -1;
    }
    else {
        int numParametersPerTap = GDP_TAP_B_ENABLE - GDP_TAP_A_ENABLE;
        switchableIndex = (parameterIndex - GDP_TAP_A_ENABLE) % numParametersPerTap + GDP_TAP_A_ENABLE;
        tapNumber = (parameterIndex - GDP_TAP_A_ENABLE) / numParametersPerTap;
    }
}

int Editor::Impl::recomposeParameterIndex(int switchableIndex, int tapNumber)
{
    int parameterIndex;
    if (switchableIndex < GDP_TAP_A_ENABLE)
        parameterIndex = switchableIndex;
    else
        parameterIndex = switchableIndex + tapNumber * (GDP_TAP_B_ENABLE - GDP_TAP_A_ENABLE);
    return parameterIndex;
}

void Editor::Impl::tapEditStarted(TapEditScreen *, int tapNumber, ChangeId id)
{
    int parameterIndex = recomposeParameterIndex((int)id, tapNumber);
    juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);

    parameter->beginChangeGesture();

    setActiveTap(tapNumber);
}

void Editor::Impl::tapEditEnded(TapEditScreen *, int tapNumber, ChangeId id)
{
    int parameterIndex = recomposeParameterIndex((int)id, tapNumber);
    juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);

    parameter->endChangeGesture();

    setActiveTap(tapNumber);
}

void Editor::Impl::tapValueChanged(TapEditScreen *, int tapNumber, ChangeId id, float value)
{
    int parameterIndex = recomposeParameterIndex((int)id, tapNumber);
    juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);

    float valueNormalized = parameter->convertTo0to1(value);

    parameter->setValueNotifyingHost(valueNormalized);

    if (activeTapNumber_ != tapNumber)
        setActiveTap(tapNumber);
    else
        syncActiveTapParameterToControls((int)id);
}

void Editor::Impl::sliderDragStarted(juce::Slider *slider)
{
    int parameterIndex = getParameterForSlider(slider);

    if (parameterIndex != -1) {
        juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);
        parameter->beginChangeGesture();
    }
}

void Editor::Impl::sliderDragEnded(juce::Slider *slider)
{
    int parameterIndex = getParameterForSlider(slider);

    if (parameterIndex != -1) {
        juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);
        parameter->endChangeGesture();
    }
}

void Editor::Impl::sliderValueChanged(juce::Slider *slider)
{
    int parameterIndex = getParameterForSlider(slider);

    if (parameterIndex != -1) {
        juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);
        float value = parameter->convertTo0to1((float)slider->getValue());
        parameter->setValueNotifyingHost(value);
    }
}

void Editor::Impl::buttonClicked(juce::Button *button)
{
    (void)button;
}

void Editor::Impl::buttonStateChanged(juce::Button *button)
{
    int parameterIndex = getParameterForButton(button);

    if (parameterIndex != -1) {
        juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);
        parameter->setValueNotifyingHost(button->getToggleState());
    }
}

void Editor::Impl::comboBoxChanged(juce::ComboBox *comboBox)
{
    int parameterIndex = getParameterForComboBox(comboBox);

    if (parameterIndex != -1) {
        juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);
        float value = parameter->convertTo0to1((float)comboBox->getSelectedItemIndex());
        parameter->setValueNotifyingHost(value);
    }
}

void Editor::Impl::parameterValueChanged(int parameterIndex, float newValueNormalized)
{
    MainComponent &mainComponent = *mainComponent_;
    TapEditScreen *tapEdit = mainComponent.getTapEditScreen();
    juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters_[parameterIndex]);

    float newValue = parameter->convertFrom0to1(newValueNormalized);

    int switchableIndex;
    int tapNumber;
    breakDownParameterIndex(parameterIndex, switchableIndex, tapNumber);

    switch (switchableIndex) {
    case GDP_FEEDBACK_TAP:
        mainComponent.getFeedbackTapChoice()->setSelectedItemIndex((int)newValue, juce::dontSendNotification);
        break;
    case GDP_FEEDBACK_GAIN:
        mainComponent.getFeedbackTapGainSlider()->setValue(newValue, juce::dontSendNotification);
        break;
    case GDP_MIX_DRY:
        mainComponent.getDrySlider()->setValue(newValue, juce::dontSendNotification);
        break;
    case GDP_MIX_WET:
        mainComponent.getWetSlider()->setValue(newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_ENABLE:
        tapEdit->setTapEnabled(tapNumber, (bool)newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_DELAY:
        tapEdit->setTapDelay(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_LPF_CUTOFF:
        tapEdit->setTapLPFCutoff(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_HPF_CUTOFF:
        tapEdit->setTapHPFCutoff(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_RESONANCE:
        tapEdit->setTapResonance(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_TUNE:
        tapEdit->setTapTune(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_LEVEL:
        tapEdit->setTapLevel(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_PAN:
        tapEdit->setTapPan(tapNumber, newValue, juce::dontSendNotification);
        break;
    case GDP_TAP_A_WIDTH:
        // TODO
        break;
    }

    if (tapNumber == activeTapNumber_)
        syncActiveTapParameterToControls(switchableIndex);
}

void Editor::Impl::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    (void)parameterIndex;
    (void)gestureIsStarting;
}
