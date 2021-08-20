// SPDX-License-Identifier: BSD-2-Clause
#include "editor/Editor.h"
#include "editor/LookAndFeel.h"
#include "editor/parts/MainComponent.h"
#include "editor/parts/TapEditScreen.h"
#include "editor/attachments/TapParameterAttachment.h"
#include "editor/attachments/AutomaticComboBoxParameterAttachment.h"
#include "processor/Processor.h"
#include "GdDefs.h"
#include <vector>

//==============================================================================
struct Editor::Impl : public TapEditScreen::Listener {
    Editor *self_ = nullptr;
    std::unique_ptr<MainComponent> mainComponent_;
    juce::Array<juce::AudioProcessorParameter *> parameters_;
    int activeTapNumber_ = -1;

    std::vector<std::unique_ptr<TapParameterAttachment>> tapAttachements_;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> sliderAttachements_;
    std::vector<std::unique_ptr<AutomaticComboBoxParameterAttachment>> comboBoxAttachements_;

    struct ActiveTapAttachments {
        std::vector<std::unique_ptr<juce::SliderParameterAttachment>> sliderAttachements_;
        std::vector<std::unique_ptr<juce::ButtonParameterAttachment>> buttonAttachements_;
        std::vector<std::unique_ptr<AutomaticComboBoxParameterAttachment>> comboBoxAttachements_;
    };
    ActiveTapAttachments activeTapAttachments_;

    juce::RangedAudioParameter *getRangedParameter(int i) const {
        return static_cast<juce::RangedAudioParameter *>(parameters_[i]);
    }

    void setActiveTap(int tapNumber);
    void createActiveTapParameterAttachments();

    void tapEditStarted(TapEditScreen *, GdParameter id) override;
    void tapEditEnded(TapEditScreen *, GdParameter id) override;
    void tapValueChanged(TapEditScreen *, GdParameter id, float value) override;
};

//==============================================================================
Editor::Editor(Processor &p)
    : AudioProcessorEditor(p),
      impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    impl.parameters_ = p.getParameters();

    static LookAndFeel lnf;
    setLookAndFeel(&lnf);
    juce::LookAndFeel::setDefaultLookAndFeel(&lnf);

    MainComponent *mainComponent = new MainComponent;
    impl.mainComponent_.reset(mainComponent);

    setSize(mainComponent->getWidth(), mainComponent->getHeight());
    addAndMakeVisible(mainComponent);

    TapEditScreen &tapEdit = *mainComponent->tapEditScreen_;
    tapEdit.addListener(&impl);

    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        const GdParameter decomposedIds[] = {
            GDP_TAP_A_ENABLE,
            GDP_TAP_A_DELAY,
            GDP_TAP_A_LPF_CUTOFF,
            GDP_TAP_A_HPF_CUTOFF,
            GDP_TAP_A_RESONANCE,
            GDP_TAP_A_TUNE,
            GDP_TAP_A_PAN,
            GDP_TAP_A_LEVEL,
        };
        for (GdParameter decomposedId : decomposedIds) {
            GdParameter id = GdRecomposeParameter(decomposedId, tapNumber);
            impl.tapAttachements_.emplace_back(new TapParameterAttachment(*impl.getRangedParameter((int)id), tapEdit));
        }
    }

    impl.sliderAttachements_.emplace_back(new juce::SliderParameterAttachment(*impl.getRangedParameter((int)GDP_FEEDBACK_GAIN), *mainComponent->feedbackTapGainSlider_, nullptr));
    impl.comboBoxAttachements_.emplace_back(new AutomaticComboBoxParameterAttachment(*impl.getRangedParameter((int)GDP_FEEDBACK_TAP), *mainComponent->feedbackTapChoice_, nullptr));
    impl.sliderAttachements_.emplace_back(new juce::SliderParameterAttachment(*impl.getRangedParameter((int)GDP_MIX_WET), *mainComponent->wetSlider_, nullptr));
    impl.sliderAttachements_.emplace_back(new juce::SliderParameterAttachment(*impl.getRangedParameter((int)GDP_MIX_DRY), *mainComponent->drySlider_, nullptr));

    impl.setActiveTap(0);
}

Editor::~Editor()
{
}

void Editor::Impl::setActiveTap(int tapNumber)
{
    if (activeTapNumber_ == tapNumber)
        return;

    activeTapNumber_ = tapNumber;

    ///
    MainComponent &mainComponent = *mainComponent_;
    char tapChar = (char)('A' + tapNumber);

    juce::String tapFormat = TRANS("Tap %c");
    char tapText[256];
    std::sprintf(tapText, tapFormat.toRawUTF8(), tapChar);
    mainComponent.setActiveTapLabelText(tapText);

    ///
    createActiveTapParameterAttachments();
}

void Editor::Impl::createActiveTapParameterAttachments()
{
    ActiveTapAttachments &ata = activeTapAttachments_;
    int tapNumber = activeTapNumber_;
    MainComponent *mainComponent = mainComponent_.get();

    ata = ActiveTapAttachments{};
    ata.buttonAttachements_.emplace_back(new juce::ButtonParameterAttachment(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber)), *mainComponent->tapEnabledButton_, nullptr));
    ata.sliderAttachements_.emplace_back(new juce::SliderParameterAttachment(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_DELAY, tapNumber)), *mainComponent->tapDelaySlider_, nullptr));
    ata.comboBoxAttachements_.emplace_back(new AutomaticComboBoxParameterAttachment(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_FILTER, tapNumber)), *mainComponent->filterChoice_, nullptr));
}

void Editor::Impl::tapEditStarted(TapEditScreen *, GdParameter id)
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);
    setActiveTap(tapNumber);
}

void Editor::Impl::tapEditEnded(TapEditScreen *, GdParameter id)
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);
    setActiveTap(tapNumber);
}

void Editor::Impl::tapValueChanged(TapEditScreen *, GdParameter id, float)
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);
    setActiveTap(tapNumber);
}
