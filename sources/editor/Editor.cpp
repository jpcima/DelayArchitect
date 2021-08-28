// SPDX-License-Identifier: BSD-2-Clause
#include "editor/Editor.h"
#include "editor/LookAndFeel.h"
#include "editor/parts/MainComponent.h"
#include "editor/parts/TapEditScreen.h"
#include "editor/parts/AdvancedTooltipWindow.h"
#include "editor/attachments/TapParameterAttachment.h"
#include "editor/attachments/GridParameterAttachment.h"
#include "editor/attachments/AutomaticComboBoxParameterAttachment.h"
#include "editor/attachments/InvertedButtonParameterAttachment.h"
#include "editor/attachments/SliderParameterAttachmentWithTooltip.h"
#include "editor/utility/FunctionalTimer.h"
#include "processor/Processor.h"
#include "importer/ImporterPST.h"
#include "utility/AutoDeletePool.h"
#include "Gd.h"
#include <vector>

//==============================================================================
struct Editor::Impl : public TapEditScreen::Listener {
    Editor *self_ = nullptr;
    Processor *processor_ = nullptr;
    std::unique_ptr<AdvancedTooltipWindow> tooltipWindow_;
    std::unique_ptr<MainComponent> mainComponent_;
    juce::Array<juce::AudioProcessorParameter *> parameters_;
    int activeTapNumber_ = -1;

    AutoDeletePool globalAttachments_;
    AutoDeletePool activeTapAttachments_;

    std::unique_ptr<juce::PopupMenu> mainMenu_;

    std::unique_ptr<juce::FileChooser> fileChooser_;

    std::unique_ptr<juce::Timer> idleTimer_;

    juce::RangedAudioParameter *getRangedParameter(int i) const {
        return static_cast<juce::RangedAudioParameter *>(parameters_[i]);
    }

    void runIdle();

    void setActiveTap(int tapNumber);
    void createActiveTapParameterAttachments();

    void choosePresetFileToImport();
    void importPresetFile(const juce::File &file);

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
    impl.processor_ = &p;

    impl.parameters_ = p.getParameters();

    static LookAndFeel lnf;
    setLookAndFeel(&lnf);
    juce::LookAndFeel::setDefaultLookAndFeel(&lnf);

    AdvancedTooltipWindow *tooltipWindow = new AdvancedTooltipWindow;
    impl.tooltipWindow_.reset(tooltipWindow);

    MainComponent *mainComponent = new MainComponent;
    impl.mainComponent_.reset(mainComponent);

    juce::PopupMenu *mainMenu = new juce::PopupMenu;
    impl.mainMenu_.reset(mainMenu);
    mainMenu->addItem(TRANS("Import preset"), [&impl]() { impl.choosePresetFileToImport(); });
    mainComponent->menuButton_->onClick = [this]() {
        impl_->mainMenu_->showMenuAsync(
            juce::PopupMenu::Options()
            .withParentComponent(this)
            .withTargetComponent(impl_->mainComponent_->menuButton_.get()));
    };

    setSize(mainComponent->getWidth(), mainComponent->getHeight());
    addAndMakeVisible(mainComponent);

    TapEditScreen &tapEdit = *mainComponent->tapEditScreen_;
    tapEdit.addListener(&impl);

    //
    AutoDeletePool &att = impl.globalAttachments_;

    for (int i = 0; i < GD_PARAMETER_COUNT; ++i) {
        juce::RangedAudioParameter &parameter = *impl.getRangedParameter(i);
        att.makeNew<TapParameterAttachment>(parameter, tapEdit);
    }

    att.makeNew<juce::ButtonParameterAttachment>(*impl.getRangedParameter((int)GDP_SYNC), *mainComponent->syncButton_, nullptr);
    att.makeNew<juce::ButtonParameterAttachment>(*impl.getRangedParameter((int)GDP_FEEDBACK_ENABLE), *mainComponent->feedbackEnableButton_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*impl.getRangedParameter((int)GDP_FEEDBACK_GAIN), *mainComponent->feedbackTapGainSlider_, nullptr);
    att.makeNew<AutomaticComboBoxParameterAttachment>(*impl.getRangedParameter((int)GDP_FEEDBACK_TAP), *mainComponent->feedbackTapChoice_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*impl.getRangedParameter((int)GDP_SWING), *mainComponent->swingSlider_, nullptr);
    att.makeNew<SliderParameterAttachmentWithTooltip>(*impl.getRangedParameter((int)GDP_MIX_WET), *mainComponent->wetSlider_, *impl.tooltipWindow_, nullptr);
    att.makeNew<SliderParameterAttachmentWithTooltip>(*impl.getRangedParameter((int)GDP_MIX_DRY), *mainComponent->drySlider_, *impl.tooltipWindow_, nullptr);
    att.makeNew<GridParameterAttachment>(*impl.getRangedParameter((int)GDP_GRID), *mainComponent->gridChoice_);

    //
    impl.setActiveTap(0);

    //
    juce::Timer *idleTimer = FunctionalTimer::create([&impl]() { impl.runIdle(); });
    impl.idleTimer_.reset(idleTimer);
    idleTimer->startTimer(50);
}

Editor::~Editor()
{
}

void Editor::Impl::runIdle()
{
    double bpm = processor_->getLastKnownBPM();
    mainComponent_->tapEditScreen_->setBPM(bpm);
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
    AutoDeletePool &att = activeTapAttachments_;
    int tapNumber = activeTapNumber_;
    MainComponent *mainComponent = mainComponent_.get();

    att.clear();
    att.makeNew<juce::ButtonParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber)), *mainComponent->tapEnabledButton_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_DELAY, tapNumber)), *mainComponent->tapDelaySlider_, nullptr);
    att.makeNew<juce::ButtonParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_FILTER_ENABLE, tapNumber)), *mainComponent->filterEnableButton_, nullptr);
    att.makeNew<AutomaticComboBoxParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_FILTER, tapNumber)), *mainComponent->filterChoice_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_HPF_CUTOFF, tapNumber)), *mainComponent->hpfCutoffSlider_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_LPF_CUTOFF, tapNumber)), *mainComponent->lpfCutoffSlider_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_RESONANCE, tapNumber)), *mainComponent->resonanceSlider_, nullptr);
    att.makeNew<juce::ButtonParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_TUNE_ENABLE, tapNumber)), *mainComponent->tuneEnableButton_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_TUNE, tapNumber)), *mainComponent->pitchSlider_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_PAN, tapNumber)), *mainComponent->panSlider_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_WIDTH, tapNumber)), *mainComponent->widthSlider_, nullptr);
    att.makeNew<juce::ButtonParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_FLIP, tapNumber)), *mainComponent->flipEnableButton_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_LEVEL, tapNumber)), *mainComponent->levelSlider_, nullptr);
    att.makeNew<InvertedButtonParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_MUTE, tapNumber)), *mainComponent->muteButton_, nullptr);
}

void Editor::Impl::choosePresetFileToImport()
{
    Editor *self = self_;

    juce::FileChooser *chooser = new juce::FileChooser(
        TRANS("Import preset"), {}, "*.pst", true, true, self);
    fileChooser_.reset(chooser);

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser &theChooser) {
            juce::File result = theChooser.getResult();
            if (result != juce::File{})
                importPresetFile(result.getFullPathName());
        });
}

void Editor::Impl::importPresetFile(const juce::File &file)
{
    ImporterPST pst;
    ImportData idata;
    if (!pst.importFile(file, idata)) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, TRANS("Error"), TRANS("Could not import the preset file."), juce::String{}, self_);
        return;
    }

    juce::AudioProcessorParameter **parameters = parameters_.data();
    for (uint32_t p = 0; p < GD_PARAMETER_COUNT; ++p) {
        juce::RangedAudioParameter *parameter = static_cast<juce::RangedAudioParameter *>(parameters[p]);
        parameter->setValueNotifyingHost(parameter->convertTo0to1(idata.values[p]));
    }

    MainComponent *mainComponent = mainComponent_.get();
    TapEditScreen *tapEdit = mainComponent->tapEditScreen_.get();
    tapEdit->autoZoomTimeRange();
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
