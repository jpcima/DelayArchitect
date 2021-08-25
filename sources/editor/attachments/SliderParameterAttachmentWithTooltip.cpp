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
