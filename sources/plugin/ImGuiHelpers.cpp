// SPDX-License-Identifier: BSD-2-Clause
#include "ImGuiHelpers.h"
#include <GL/glew.h>

namespace ImGuiHelpers {
    void initialiseOpenGLloader()
    {
        glewInit();
    }
}
