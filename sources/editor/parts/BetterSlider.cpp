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

#include "BetterSlider.h"

namespace better {

void Slider::mouseWheelMove(const juce::MouseEvent &e, const juce::MouseWheelDetails &wheel)
{
    juce::MouseWheelDetails newWheel = wheel;

    // we want small value increments when ctrl or alt is pressed
    if (e.mods.testFlags(juce::ModifierKeys::ctrlAltCommandModifiers)) {
        // XXX check if slider would use the event (according to JUCE Slider.cpp)
        bool sliderWillUseWheelEvent = isEnabled() && isScrollWheelEnabled() && !isTwoValue();
        if (sliderWillUseWheelEvent) {
            // XXX pass a smaller wheel amount; the default increment is 15% of range.
            //     change this into virtually 1% of total range.
            float factor = 1.0f / 15.0f;
            newWheel.deltaX *= factor;
            newWheel.deltaY *= factor;
        }
    }

    juce::Slider::mouseWheelMove(e, newWheel);
}

} // namespace better
