#include "InvertedButtonParameterAttachment.h"

InvertedButtonParameterAttachment::InvertedButtonParameterAttachment(juce::RangedAudioParameter &param, juce::Button &b, juce::UndoManager *um)
    : button_(b),
      attachment_(param, [this](float f) { setValue(f); }, um)
{
    sendInitialUpdate();
    button_.addListener(this);
}

InvertedButtonParameterAttachment::~InvertedButtonParameterAttachment()
{
    button_.removeListener(this);
}

void InvertedButtonParameterAttachment::sendInitialUpdate()
{
    attachment_.sendInitialUpdate();
}

void InvertedButtonParameterAttachment::setValue(float newValue)
{
    const juce::ScopedValueSetter<bool> svs(ignoreCallbacks_, true);
    button_.setToggleState(newValue < 0.5f, juce::sendNotificationSync);
}

void InvertedButtonParameterAttachment::buttonClicked(juce::Button *)
{
    if (ignoreCallbacks_)
        return;

    attachment_.setValueAsCompleteGesture(button_.getToggleState() ? 0.0f : 1.0f);
}
