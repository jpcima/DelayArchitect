/* Copyright (c) 2021, Jean Pierre Cimalando
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SliderParameterAttachmentWithTooltip.h"
#include "editor/parts/AdvancedTooltipWindow.h"

struct SliderParameterAttachmentWithTooltip::Impl : public juce::Slider::Listener {
public:
    juce::RangedAudioParameter *parameter_ = nullptr;
    juce::Slider *slider_ = nullptr;
    AdvancedTooltipWindow *tooltipWindow_ = nullptr;
    bool isDragging_ = false;

protected:
    void sliderValueChanged(juce::Slider *slider) override;
    void sliderDragStarted(juce::Slider *slider) override;
    void sliderDragEnded(juce::Slider *slider) override;
};

SliderParameterAttachmentWithTooltip::SliderParameterAttachmentWithTooltip(juce::RangedAudioParameter &parameter, juce::Slider &slider, AdvancedTooltipWindow &tooltipWindow, juce::UndoManager *undoManager)
    : juce::SliderParameterAttachment(parameter, slider, undoManager),
      impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.parameter_ = &parameter;
    impl.slider_ = &slider;
    impl.tooltipWindow_ = &tooltipWindow;
    slider.addListener(&impl);
}

SliderParameterAttachmentWithTooltip::~SliderParameterAttachmentWithTooltip()
{
    Impl &impl = *impl_;
    impl.slider_->removeListener(&impl);
}

void SliderParameterAttachmentWithTooltip::Impl::sliderValueChanged(juce::Slider *slider)
{
    if (isDragging_) {
        juce::Desktop &desktop = juce::Desktop::getInstance();
        juce::MouseInputSource mouseSource = desktop.getMainMouseSource();
        juce::Point<float> mousePos = mouseSource.getScreenPosition();
        float value = (float)slider->getValue();
        juce::String newTip = parameter_->getText(parameter_->convertTo0to1(value), 0);
        tooltipWindow_->setPersistentText(newTip);
        tooltipWindow_->displayTip(mousePos.roundToInt(), newTip);
    }
}

void SliderParameterAttachmentWithTooltip::Impl::sliderDragStarted(juce::Slider *slider)
{
    (void)slider;
    isDragging_ = true;
}

void SliderParameterAttachmentWithTooltip::Impl::sliderDragEnded(juce::Slider *slider)
{
    (void)slider;
    isDragging_ = false;
    tooltipWindow_->clearPersistentText();
}
