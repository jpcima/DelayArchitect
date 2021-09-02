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

#include "editor/Editor.h"
#include "editor/LookAndFeel.h"
#include "editor/parts/MainComponent.h"
#include "editor/parts/AboutComponent.h"
#include "editor/parts/TapEditScreen.h"
#include "editor/parts/AdvancedTooltipWindow.h"
#include "editor/attachments/TapParameterAttachment.h"
#include "editor/attachments/GridParameterAttachment.h"
#include "editor/attachments/AutomaticComboBoxParameterAttachment.h"
#include "editor/attachments/InvertedButtonParameterAttachment.h"
#include "editor/attachments/SliderParameterAttachmentWithTooltip.h"
#include "editor/attachments/ComboBoxParameterAttachmentByID.h"
#include "editor/utility/FunctionalTimer.h"
#include "processor/Processor.h"
#include "processor/PresetFile.h"
#include "importer/ImporterPST.h"
#include "utility/AutoDeletePool.h"
#include "Gd.h"
#include <vector>

//==============================================================================
struct Editor::Impl : public TapEditScreen::Listener,
                      public juce::AudioProcessorParameter::Listener {
    Editor *self_ = nullptr;
    Processor *processor_ = nullptr;
    std::unique_ptr<AdvancedTooltipWindow> tooltipWindow_;
    std::unique_ptr<MainComponent> mainComponent_;
    juce::Array<juce::AudioProcessorParameter *> parameters_;
    int activeTapNumber_ = -1;

    AutoDeletePool globalAttachments_;
    AutoDeletePool activeTapAttachments_;

    std::unique_ptr<juce::PopupMenu> mainMenu_;
    std::unique_ptr<juce::PopupMenu> tapMenu_;

    std::unique_ptr<juce::FileChooser> fileChooser_;
    juce::File fileChooserInitialDirectory_;

    std::unique_ptr<juce::Timer> idleTimer_;

    std::atomic<int> flagTapEnablementChanged_{0};

    juce::RangedAudioParameter *getRangedParameter(int i) const {
        return static_cast<juce::RangedAudioParameter *>(parameters_[i]);
    }

    void runIdle();

    void setActiveTap(int tapNumber);
    void createActiveTapParameterAttachments();

    void updateTapChoiceComboBoxes();
    void updateTapChoiceComboBox(juce::ComboBox &combo);

    void choosePresetFileToLoad();
    void choosePresetFileToSave();
    void choosePresetFileToImport();
    void loadPresetFile(const juce::File &file);
    void savePresetFile(const juce::File &file);
    void importPresetFile(const juce::File &file);
    void loadPreset(const PresetFile &pst);

    void copyActiveTap();
    void pasteActiveTap();

    void showAbout();

    void tapEditStarted(TapEditScreen *, GdParameter id) override;
    void tapEditEnded(TapEditScreen *, GdParameter id) override;
    void tapValueChanged(TapEditScreen *, GdParameter id, float value) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
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

    mainComponent->logoButton_->onClick = [&impl]() { impl.showAbout(); };

    juce::PopupMenu *mainMenu = new juce::PopupMenu;
    impl.mainMenu_.reset(mainMenu);
    mainMenu->addItem(TRANS("Load patch"), [&impl]() { impl.choosePresetFileToLoad(); });
    mainMenu->addItem(TRANS("Save patch"), [&impl]() { impl.choosePresetFileToSave(); });
    mainMenu->addSeparator();
    mainMenu->addItem(TRANS("Import patch"), [&impl]() { impl.choosePresetFileToImport(); });
    mainMenu->addSeparator();
    mainMenu->addItem(TRANS("Recall defaults"), [&impl]() { impl.loadPreset(PresetFile::makeDefault()); });
    mainMenu->addSeparator();
    mainMenu->addItem(TRANS("About"), [&impl]() { impl.showAbout(); });
    mainComponent->menuButton_->onClick = [this]() {
        impl_->mainMenu_->showMenuAsync(
            juce::PopupMenu::Options()
            .withParentComponent(this)
            .withTargetComponent(impl_->mainComponent_->menuButton_.get()));
    };

    juce::PopupMenu *tapMenu = new juce::PopupMenu;
    impl.tapMenu_.reset(tapMenu);
    tapMenu->addItem(TRANS("Copy tap"), [&impl]() { impl.copyActiveTap(); });
    tapMenu->addItem(TRANS("Paste tap"), [&impl]() { impl.pasteActiveTap(); });
    mainComponent->tapMenuButton_->onClick = [this]() {
        impl_->tapMenu_->showMenuAsync(
            juce::PopupMenu::Options()
            .withParentComponent(this)
            .withTargetComponent(impl_->mainComponent_->tapMenuButton_.get()));
    };

    setSize(mainComponent->getWidth(), mainComponent->getHeight());
    addAndMakeVisible(mainComponent);

    TapEditScreen &tapEdit = *mainComponent->tapEditScreen_;
    tapEdit.addListener(&impl);

    //
    impl.updateTapChoiceComboBoxes();

    //
    AutoDeletePool &att = impl.globalAttachments_;

    for (int i = 0; i < GD_PARAMETER_COUNT; ++i) {
        juce::RangedAudioParameter &parameter = *impl.getRangedParameter(i);
        att.makeNew<TapParameterAttachment>(parameter, tapEdit);
    }

    att.makeNew<juce::ButtonParameterAttachment>(*impl.getRangedParameter((int)GDP_SYNC), *mainComponent->syncButton_, nullptr);
    att.makeNew<juce::ButtonParameterAttachment>(*impl.getRangedParameter((int)GDP_FEEDBACK_ENABLE), *mainComponent->feedbackEnableButton_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*impl.getRangedParameter((int)GDP_FEEDBACK_GAIN), *mainComponent->feedbackTapGainSlider_, nullptr);
    att.makeNew<ComboBoxParameterAttachmentByID>(*impl.getRangedParameter((int)GDP_FEEDBACK_TAP), *mainComponent->feedbackTapChoice_, nullptr);
    att.makeNew<juce::SliderParameterAttachment>(*impl.getRangedParameter((int)GDP_SWING), *mainComponent->swingSlider_, nullptr);
    att.makeNew<SliderParameterAttachmentWithTooltip>(*impl.getRangedParameter((int)GDP_MIX_WET), *mainComponent->wetSlider_, *impl.tooltipWindow_, nullptr);
    att.makeNew<SliderParameterAttachmentWithTooltip>(*impl.getRangedParameter((int)GDP_MIX_DRY), *mainComponent->drySlider_, *impl.tooltipWindow_, nullptr);
    att.makeNew<GridParameterAttachment>(*impl.getRangedParameter((int)GDP_GRID), *mainComponent->gridChoice_);

    //
    for (int i = 0; i < GD_PARAMETER_COUNT; ++i) {
        juce::RangedAudioParameter &parameter = *impl.getRangedParameter(i);
        parameter.addListener(&impl);
    }

    //
    mainComponent->activeTapChoice_->onChange = [&impl]() {
        int id = impl.mainComponent_->activeTapChoice_->getSelectedId();
        if (id > 0)
            impl.setActiveTap(id - 1);
    };

    //
    mainComponent->patchNameEditor_->onTextChange = [&impl]() {
        juce::String presetName = impl.mainComponent_->patchNameEditor_->getText();
        impl.processor_->setCurrentPresetName(presetName);
    };

    //
    impl.setActiveTap(0);

    //
    juce::Timer *idleTimer = FunctionalTimer::create([&impl]() { impl.runIdle(); });
    impl.idleTimer_.reset(idleTimer);
    idleTimer->startTimer(50);

    //
    syncStateFromProcessor();
}

Editor::~Editor()
{
    Impl &impl = *impl_;

    for (int i = 0; i < GD_PARAMETER_COUNT; ++i) {
        juce::RangedAudioParameter &parameter = *impl.getRangedParameter(i);
        parameter.removeListener(&impl);
    }
}

void Editor::syncStateFromProcessor()
{
    Impl &impl = *impl_;

    juce::String presetName = impl.processor_->getCurrentPresetName();
    impl.mainComponent_->patchNameEditor_->setText(presetName, false);
}

void Editor::Impl::runIdle()
{
    double bpm = processor_->getLastKnownBPM();
    mainComponent_->tapEditScreen_->setBPM(bpm);

    int flagValue = 1;
    if (flagTapEnablementChanged_.compare_exchange_weak(flagValue, 0, std::memory_order_relaxed))
        updateTapChoiceComboBoxes();
}

void Editor::Impl::setActiveTap(int tapNumber)
{
    if (activeTapNumber_ == tapNumber)
        return;

    activeTapNumber_ = tapNumber;

    ///
    MainComponent &mainComponent = *mainComponent_;
    juce::ComboBox &activeTapChoice = *mainComponent.activeTapChoice_;
    activeTapChoice.setSelectedId(tapNumber + 1, juce::dontSendNotification);

    ///
    TapEditScreen &screen = *mainComponent.tapEditScreen_;
    screen.setOnlyTapSelected(tapNumber);

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
    att.makeNew<juce::ButtonParameterAttachment>(*getRangedParameter((int)GdRecomposeParameter(GDP_TAP_A_MUTE, tapNumber)), *mainComponent->muteButton_, nullptr);
}

void Editor::Impl::updateTapChoiceComboBoxes()
{
    updateTapChoiceComboBox(*mainComponent_->activeTapChoice_);
    updateTapChoiceComboBox(*mainComponent_->feedbackTapChoice_);
}

void Editor::Impl::updateTapChoiceComboBox(juce::ComboBox &combo)
{
    bool tapEnabled[GdMaxLines];
    bool anyTapEnabled = false;

    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        GdParameter id = GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber);
        bool enabled = tapEnabled[tapNumber] = (bool)getRangedParameter((int)id)->getValue();
        anyTapEnabled |= enabled;
    }

    int selectedId = combo.getSelectedId();
    combo.clear(juce::dontSendNotification);

    juce::PopupMenu *menu = combo.getRootMenu();

    auto getTapText = [](int tapNumber) {
        char tapChar = (char)('A' + tapNumber);
        juce::String tapFormat = TRANS("Tap %s");
        return tapFormat.replaceFirstOccurrenceOf("%s", juce::String(&tapChar, 1));
    };

    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        if (tapEnabled[tapNumber])
            menu->addItem(tapNumber + 1, getTapText(tapNumber));
    }

    if (anyTapEnabled)
        menu->addSeparator();

    juce::PopupMenu subMenu;
    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        if (!tapEnabled[tapNumber])
            subMenu.addItem(tapNumber + 1, getTapText(tapNumber));
    }
    menu->addSubMenu(TRANS("Not used"), subMenu, true);

    combo.setSelectedId(selectedId, juce::dontSendNotification);
}

void Editor::Impl::choosePresetFileToLoad()
{
    Editor *self = self_;

    juce::FileChooser *chooser = new juce::FileChooser(
        TRANS("Load preset"), fileChooserInitialDirectory_,
        "*.dap", true, true, self);
    fileChooser_.reset(chooser);

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser &theChooser) {
            juce::File result = theChooser.getResult();
            if (result != juce::File{}) {
                fileChooserInitialDirectory_ = result.getParentDirectory();
                loadPresetFile(result.getFullPathName());
            }
        });
}

void Editor::Impl::choosePresetFileToSave()
{
    Editor *self = self_;

    juce::FileChooser *chooser = new juce::FileChooser(
        TRANS("Save preset"), fileChooserInitialDirectory_,
        "*.dap", true, true, self);
    fileChooser_.reset(chooser);

    chooser->launchAsync(
        juce::FileBrowserComponent::saveMode|juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser &theChooser) {
            juce::File result = theChooser.getResult();
            if (result != juce::File{}) {
                fileChooserInitialDirectory_ = result.getParentDirectory();
                if (!result.hasFileExtension("dap"))
                    result = result.withFileExtension("dap");
                if (!result.exists())
                    savePresetFile(result.getFullPathName());
                else {
                    juce::MessageBoxOptions opt = juce::MessageBoxOptions{}
                        .withTitle(TRANS("Confirm overwrite"))
                        .withMessage(TRANS("The file already exists. Would you like to replace it?"))
                        .withButton(TRANS("Yes")).withButton(TRANS("No"))
                        .withAssociatedComponent(self_);
                    juce::AlertWindow::showAsync(opt, [this, result](int button) {
                        if (button == 1)
                            savePresetFile(result.getFullPathName());
                    });
                }
            }
        });
}

void Editor::Impl::choosePresetFileToImport()
{
    Editor *self = self_;

    juce::FileChooser *chooser = new juce::FileChooser(
        TRANS("Import preset"), fileChooserInitialDirectory_,
        "*.pst", true, true, self);
    fileChooser_.reset(chooser);

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser &theChooser) {
            juce::File result = theChooser.getResult();
            if (result != juce::File{}) {
                fileChooserInitialDirectory_ = result.getParentDirectory();
                importPresetFile(result.getFullPathName());
            }
        });
}

void Editor::Impl::loadPresetFile(const juce::File &file)
{
    PresetFile pst = PresetFile::loadFromFile(file);
    if (!pst) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, TRANS("Error"), TRANS("Could not load the preset file."), juce::String{}, self_);
        return;
    }

    loadPreset(pst);
}

void Editor::Impl::savePresetFile(const juce::File &file)
{
    PresetFile pst;
    pst.valid = true;
    pst.name = PresetFile::nameFromString(processor_->getCurrentPresetName());

    for (int i = 0; i < GD_PARAMETER_COUNT; ++i) {
        juce::RangedAudioParameter *parameter = getRangedParameter(i);
        pst.values[i] = parameter->convertFrom0to1(parameter->getValue());
    }

    if (!PresetFile::saveToFile(pst, file)) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, TRANS("Error"), TRANS("Could not save the preset file."), juce::String{}, self_);
        return;
    }
}

void Editor::Impl::importPresetFile(const juce::File &file)
{
    ImporterPST pst;
    ImportData idata;
    if (!pst.importFile(file, idata)) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, TRANS("Error"), TRANS("Could not import the preset file."), juce::String{}, self_);
        return;
    }

    loadPreset(idata.pst);
}

void Editor::Impl::loadPreset(const PresetFile &pst)
{
    for (int i = 0; i < GD_PARAMETER_COUNT; ++i) {
        juce::RangedAudioParameter *parameter = getRangedParameter(i);
        parameter->setValueNotifyingHost(parameter->convertTo0to1(pst.values[i]));
    }

    ///
    MainComponent *mainComponent = mainComponent_.get();

    TapEditScreen *tapEdit = mainComponent->tapEditScreen_.get();
    tapEdit->autoZoomTimeRange();

    juce::String presetName = PresetFile::nameToString(pst.name);
    mainComponent->patchNameEditor_->setText(presetName, juce::dontSendNotification);

    ///
    processor_->setCurrentPresetName(presetName);
}

void Editor::Impl::copyActiveTap()
{
    int activeTapNumber = activeTapNumber_;
    juce::ValueTree tree("TapParameters");

    for (int i = 0; i < GdNumPametersPerTap; ++i) {
        GdParameter decomposedId = (GdParameter)(GdFirstParameterOfFirstTap + i);
        if (decomposedId == GDP_TAP_A_ENABLE || decomposedId == GDP_TAP_A_DELAY)
            continue;

        GdParameter id = GdRecomposeParameter(decomposedId, activeTapNumber);
        juce::RangedAudioParameter *parameter = getRangedParameter((int)id);
        float value = parameter->convertFrom0to1(parameter->getValue());
        tree.setProperty(GdParameterName(decomposedId), (double)value, nullptr);
    }

    juce::SystemClipboard::copyTextToClipboard(tree.toXmlString().toRawUTF8());
}

void Editor::Impl::pasteActiveTap()
{
    int activeTapNumber = activeTapNumber_;
    juce::ValueTree tree = juce::ValueTree::fromXml(juce::SystemClipboard::getTextFromClipboard());

    if (tree.getType().toString() != "TapParameters")
        return;

    for (int i = 0; i < GdNumPametersPerTap; ++i) {
        GdParameter decomposedId = (GdParameter)(GdFirstParameterOfFirstTap + i);
        if (decomposedId == GDP_TAP_A_ENABLE || decomposedId == GDP_TAP_A_DELAY)
            continue;

        GdParameter id = GdRecomposeParameter(decomposedId, activeTapNumber);
        juce::RangedAudioParameter *parameter = getRangedParameter((int)id);
        if (const juce::var *value = tree.getPropertyPointer(GdParameterName(decomposedId)))
            parameter->setValueNotifyingHost(parameter->convertTo0to1((float)(double)*value));
    }
}

void Editor::Impl::showAbout()
{
    Editor *self = self_;
    juce::DialogWindow::LaunchOptions opt;

    opt.dialogTitle = TRANS("About");
    opt.content.set(new AboutComponent, true);
    opt.componentToCentreAround = self;
    opt.resizable = false;

    opt.launchAsync();
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

void Editor::Impl::parameterValueChanged(int parameterIndex, float newValue)
{
    (void)newValue;

    GdParameter decomposedId = GdDecomposeParameter((GdParameter)parameterIndex, nullptr);
    if (decomposedId == GDP_TAP_A_ENABLE)
        flagTapEnablementChanged_.store(1, std::memory_order_relaxed);
}

void Editor::Impl::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    (void)parameterIndex;
    (void)gestureIsStarting;
}
