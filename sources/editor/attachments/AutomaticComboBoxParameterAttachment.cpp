#include "AutomaticComboBoxParameterAttachment.h"

void AutomaticComboBoxParameterAttachment::fillComboBoxChoices(juce::ComboBox &combo, juce::AudioParameterChoice &parameter)
{
    combo.clear(juce::dontSendNotification);

    int numChoices = parameter.choices.size();
    for (int i = 0; i < numChoices; ++i) {
        const juce::String &choice = parameter.choices[i];
        combo.addItem(choice, i + 1);
    }
}
