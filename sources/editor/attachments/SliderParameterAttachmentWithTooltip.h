#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
class AdvancedTooltipWindow;

class SliderParameterAttachmentWithTooltip : public juce ::SliderParameterAttachment {
public:
    SliderParameterAttachmentWithTooltip(juce::RangedAudioParameter &parameter, juce::Slider &slider, AdvancedTooltipWindow &tooltipWindow, juce::UndoManager *undoManager = nullptr);
    ~SliderParameterAttachmentWithTooltip() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
