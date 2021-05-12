// SPDX-License-Identifier: BSD-2-Clause
#include "ImGuiComponent.h"
#include "ImGuiHelpers.h"
#include <juce_opengl/juce_opengl.h>
#include <imgui.h>
#include <imgui_impl_opengl2.h>
#include <imgui_impl_opengl3.h>
#include <functional>

enum {
    openGLVersionUsed = 2,
    //openGLVersionUsed = 3,
};

//==============================================================================
struct ImGuiComponent::Impl : public juce::OpenGLRenderer,
                              public juce::ComponentListener,
                              public juce::MouseListener {
    explicit Impl(ImGuiComponent &component)
        : component_(component)
    {
    }

    //==========================================================================
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    //==========================================================================
    void componentMovedOrResized(Component& component, bool wasMoved, bool wasResized) override;

    //==========================================================================
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) override;

    //==========================================================================
    class Repainter : public juce::Timer {
    public:
        explicit Repainter(juce::Component &component)
            : component_(component)
        {
        }

        void timerCallback()
        {
            component_.repaint();
        }

    private:
        juce::Component &component_;
    };

    //==========================================================================
    struct DeleteImGuiContext {
        void operator()(ImGuiContext *context)
        {
            ImGui::DestroyContext(context);
        }
    };

    //==========================================================================
    ImGuiComponent &component_;
    juce::OpenGLContext openGLContext_;
    std::unique_ptr<ImGuiContext, DeleteImGuiContext> imGuiContext_;
    std::unique_ptr<Repainter> openGLRepainter_;
};

//==============================================================================
ImGuiComponent::ImGuiComponent()
    : impl_(new Impl(*this))
{
    Impl &impl = *impl_;
    juce::OpenGLContext &context = impl.openGLContext_;

    context.setRenderer(&impl);
    context.attachTo(*this);
    if (openGLVersionUsed == (3))
        context.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);

    addComponentListener(&impl);
    addMouseListener(&impl, false);
}

ImGuiComponent::~ImGuiComponent()
{
    Impl &impl = *impl_;

    removeMouseListener(&impl);
    removeComponentListener(&impl);

    impl.openGLContext_.detach();
}

//==============================================================================
void ImGuiComponent::Impl::newOpenGLContextCreated()
{
    ImGuiHelpers::initialiseOpenGLloader();

    IMGUI_CHECKVERSION();
    imGuiContext_.reset(ImGui::CreateContext());
    ImGui::SetCurrentContext(imGuiContext_.get());

    double scaleFactor = openGLContext_.getRenderingScale();

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize.x = std::round((float)scaleFactor * (float)component_.getWidth());
    io.DisplaySize.y = std::round((float)scaleFactor * (float)component_.getHeight());
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    if (openGLVersionUsed == (2))
        ImGui_ImplOpenGL2_Init();
    else if (openGLVersionUsed == (3))
        ImGui_ImplOpenGL3_Init();

    openGLRepainter_.reset(new Repainter(component_));
    openGLRepainter_->startTimer(15);
}

void ImGuiComponent::Impl::renderOpenGL()
{
    ImGui::SetCurrentContext(imGuiContext_.get());

    if (openGLVersionUsed == (2))
        ImGui_ImplOpenGL2_NewFrame();
    else if (openGLVersionUsed == (3))
        ImGui_ImplOpenGL3_NewFrame();

    ImGui::NewFrame();

    component_.renderFrame();

    ImGui::Render();

    ImGuiIO &io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.25, 0.25, 0.25, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (openGLVersionUsed == (2))
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    else if (openGLVersionUsed == (3))
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiComponent::Impl::openGLContextClosing()
{
    openGLRepainter_.reset();

    ImGui::SetCurrentContext(imGuiContext_.get());
    if (openGLVersionUsed == (2))
        ImGui_ImplOpenGL2_Shutdown();
    else if (openGLVersionUsed == (3))
        ImGui_ImplOpenGL3_Shutdown();

    imGuiContext_.reset();
}

//==============================================================================
void ImGuiComponent::Impl::componentMovedOrResized(Component& component, bool wasMoved, bool wasResized)
{
    (void)wasMoved;

    if (wasResized) {
        if (!imGuiContext_)
            return;

        ImGui::SetCurrentContext(imGuiContext_.get());

        double scaleFactor = openGLContext_.getRenderingScale();

        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize.x = std::round((float)scaleFactor * (float)component.getWidth());
        io.DisplaySize.y = std::round((float)scaleFactor * (float)component.getHeight());
    }
}

//==============================================================================
void ImGuiComponent::Impl::mouseMove(const juce::MouseEvent &event)
{
    if (!imGuiContext_)
        return;

    ImGui::SetCurrentContext(imGuiContext_.get());

    double scaleFactor = openGLContext_.getRenderingScale();

    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = std::round((float)scaleFactor * event.position.x);
    io.MousePos.y = std::round((float)scaleFactor * event.position.y);
}

void ImGuiComponent::Impl::mouseDown(const juce::MouseEvent &event)
{
    if (!imGuiContext_)
        return;

    ImGui::SetCurrentContext(imGuiContext_.get());

    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[0] = event.mods.isLeftButtonDown();
    io.MouseDown[1] = event.mods.isRightButtonDown();
    io.MouseDown[2] = event.mods.isMiddleButtonDown();
}

void ImGuiComponent::Impl::mouseDrag(const juce::MouseEvent &event)
{
    if (!imGuiContext_)
        return;

    ImGui::SetCurrentContext(imGuiContext_.get());

    double scaleFactor = openGLContext_.getRenderingScale();

    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = std::round((float)scaleFactor * event.position.x);
    io.MousePos.y = std::round((float)scaleFactor * event.position.y);
}

void ImGuiComponent::Impl::mouseUp(const juce::MouseEvent &event)
{
    (void)event;

    if (!imGuiContext_)
        return;

    ImGui::SetCurrentContext(imGuiContext_.get());

    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[0] = false;
    io.MouseDown[1] = false;
    io.MouseDown[2] = false;
}

void ImGuiComponent::Impl::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel)
{
    (void)event;

    if (!imGuiContext_)
        return;

    ImGui::SetCurrentContext(imGuiContext_.get());

    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheel += wheel.deltaY;
    io.MouseWheelH += wheel.deltaX;
}
