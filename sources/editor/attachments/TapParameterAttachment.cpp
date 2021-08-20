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
