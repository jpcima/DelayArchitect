#pragma once
#include "editor/parts/TapEditScreen.h"
#include <juce_audio_processors/juce_audio_processors.h>

class TapParameterAttachment : public TapEditScreen::Listener {
public:
    TapParameterAttachment(juce::RangedAudioParameter &param, TapEditScreen &tapEdit);
    ~TapParameterAttachment() override;

    void sendInitialUpdate();

private:
    void setValue(float newValue);
    void tapEditStarted(TapEditScreen *, GdParameter id) override;
    void tapEditEnded(TapEditScreen *, GdParameter id) override;
    void tapValueChanged(TapEditScreen *, GdParameter id, float value) override;

private:
    TapEditScreen &tapEdit_;
    juce::RangedAudioParameter &param_;
    juce::ParameterAttachment attachment_;
    bool ignoreCallbacks_ = false;
};
