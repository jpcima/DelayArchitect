// SPDX-License-Identifier: BSD-2-Clause
#include "editor/Editor.h"
#include "processor/Processor.h"

//==============================================================================
struct Editor::Impl {
    Editor *self_ = nullptr;
};

//==============================================================================
Editor::Editor(Processor &p)
    : AudioProcessorEditor(p),
      impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    const int defaultWidth = 800;
    const int defaultHeight = 800;
    setSize(defaultWidth, defaultHeight);
}

Editor::~Editor()
{
}
