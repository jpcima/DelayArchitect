// SPDX-License-Identifier: BSD-2-Clause
#include "Editor.h"
#include "Processor.h"
#include "ImGuiComponent.h"
#include <imgui.h>

//==============================================================================
struct Editor::Impl : public juce::ComponentListener {
    explicit Impl(Editor &editor)
        : editor_(editor)
    {
    }

    //==========================================================================
    class MainComponent : public ImGuiComponent {
    public:
        explicit MainComponent(Editor::Impl *impl);
        void renderFrame() override;

    private:
        Editor::Impl *impl_ = nullptr;
    };

    //==========================================================================
    void componentMovedOrResized(Component &component, bool wasMoved, bool wasResized) override;

    //==========================================================================
    Editor &editor_;
    std::unique_ptr<MainComponent> mainComponent_;
};

//==============================================================================
Editor::Editor(Processor &p)
    : AudioProcessorEditor(p),
      impl_(new Impl(*this))
{
    Impl &impl = *impl_;
    addComponentListener(&impl);

    Impl::MainComponent *mainComponent = new Impl::MainComponent(&impl);
    impl.mainComponent_.reset(mainComponent);
    addAndMakeVisible(mainComponent);

    const int defaultWidth = 800;
    const int defaultHeight = 800;
    setSize(defaultWidth, defaultHeight);
}

Editor::~Editor()
{
    Impl &impl = *impl_;
    removeComponentListener(&impl);
}

//==============================================================================
Editor::Impl::MainComponent::MainComponent(Editor::Impl *impl)
    : impl_(impl)
{
}

void Editor::Impl::MainComponent::renderFrame()
{
    Editor::Impl &impl = *impl_;

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(500.0, 0));
    if (ImGui::Begin("Test")) {
        ImGui::Button("Click me");
        static float value = 0.5f;
        ImGui::SliderFloat("Slider 1", &value, 0.0f, 1.0f);
        ImGui::End();
    }
}

//==============================================================================
void Editor::Impl::componentMovedOrResized(Component &component, bool wasMoved, bool wasResized)
{
    (void)component;
    (void)wasMoved;

    if (wasResized) {
        mainComponent_->setSize(editor_.getWidth(), editor_.getHeight());
    }
}
