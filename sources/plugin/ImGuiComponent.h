// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class ImGuiComponent : public juce::Component
{
public:
    ImGuiComponent();
    ~ImGuiComponent();

protected:
    virtual void renderFrame() = 0;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiComponent)
};
