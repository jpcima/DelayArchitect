#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class GridParameterAttachment : private juce::ComboBox::Listener {
public:
    GridParameterAttachment(juce::RangedAudioParameter &parameter, juce::ComboBox &combo);
    ~GridParameterAttachment() override;

    void sendInitialUpdate();

private:
    void setValue(float newValue);
    void comboBoxChanged(juce::ComboBox *combo) override;

private:
    juce::ComboBox& combo_;
    juce::ParameterAttachment attachment_;
    bool ignoreCallbacks_ = false;
};
