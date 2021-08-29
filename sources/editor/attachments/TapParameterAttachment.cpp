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

#include "TapParameterAttachment.h"

TapParameterAttachment::TapParameterAttachment(juce::RangedAudioParameter &param, TapEditScreen &tapEdit)
    : tapEdit_(tapEdit),
      param_(param),
      attachment_(param, [this](float f) { setValue(f); }, nullptr)
{
    sendInitialUpdate();
    tapEdit.addListener(this);
}

TapParameterAttachment::~TapParameterAttachment()
{
    tapEdit_.removeListener(this);
}

void TapParameterAttachment::sendInitialUpdate()
{
    attachment_.sendInitialUpdate();
}

void TapParameterAttachment::setValue(float newValue)
{
    GdParameter id = (GdParameter)param_.getParameterIndex();
    const juce::ScopedValueSetter<bool> svs(ignoreCallbacks_, true);
    tapEdit_.setTapValue(id, newValue, juce::sendNotificationSync);
}

void TapParameterAttachment::tapEditStarted(TapEditScreen *, GdParameter id)
{
    if (id != (GdParameter)param_.getParameterIndex())
        return;

    attachment_.beginGesture();
}

void TapParameterAttachment::tapEditEnded(TapEditScreen *, GdParameter id)
{
    if (id != (GdParameter)param_.getParameterIndex())
        return;

    attachment_.endGesture();
}

void TapParameterAttachment::tapValueChanged(TapEditScreen *, GdParameter id, float value)
{
    if (ignoreCallbacks_)
        return;

    if (id != (GdParameter)param_.getParameterIndex())
        return;

    attachment_.setValueAsPartOfGesture(value);
}
