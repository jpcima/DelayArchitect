#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class AutomaticComboBoxParameterAttachment : public juce::ComboBoxParameterAttachment {
public:
    AutomaticComboBoxParameterAttachment(juce::RangedAudioParameter &parameter, juce::ComboBox &combo, juce::UndoManager *undoManager = nullptr)
        : juce::ComboBoxParameterAttachment(parameter, prepareComboBox(combo, parameter), undoManager)
    {
    }

private:
    static void fillComboBoxChoices(juce::ComboBox &combo, juce::AudioParameterChoice &parameter);

    static juce::ComboBox &prepareComboBox(juce::ComboBox &combo, juce::RangedAudioParameter &parameter)
    {
        jassert(dynamic_cast<juce::AudioParameterChoice *>(&parameter));
        fillComboBoxChoices(combo, static_cast<juce::AudioParameterChoice &>(parameter));
        return combo;
    }
};
