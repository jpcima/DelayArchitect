#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class InvertedButtonParameterAttachment : public juce::Button::Listener
{
public:
    InvertedButtonParameterAttachment(juce::RangedAudioParameter &parameter, juce::Button &button, juce::UndoManager *undoManager = nullptr);
    ~InvertedButtonParameterAttachment() override;
    void sendInitialUpdate();

private:
    void setValue(float newValue);
    void buttonClicked(juce::Button *) override;

    juce::Button &button_;
    juce::ParameterAttachment attachment_;
    bool ignoreCallbacks_ = false;
};
