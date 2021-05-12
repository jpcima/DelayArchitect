// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
class Processor;

class Editor : public juce::AudioProcessorEditor {
public:
    explicit Editor(Processor &processor);
    ~Editor() override;

    //==========================================================================
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};
