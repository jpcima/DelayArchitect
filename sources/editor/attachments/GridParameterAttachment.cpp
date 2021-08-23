#include "GridParameterAttachment.h"
#include "GdDefs.h"

GridParameterAttachment::GridParameterAttachment(juce::RangedAudioParameter &parameter, juce::ComboBox &combo)
    : combo_(combo),
      attachment_(parameter, [this](float f) { setValue(f); }, nullptr)
{
    combo.clear(juce::dontSendNotification);
    for (uint32_t i = 0; i < GdNumGridDivisors; ++i) {
        int div = GdGridDivisors[i];
        combo.addItem(juce::String("1/") + juce::String(div), div);
    }

    sendInitialUpdate();
    combo.addListener(this);

}

GridParameterAttachment::~GridParameterAttachment()
{
    combo_.removeListener(this);
}

void GridParameterAttachment::sendInitialUpdate()
{
    attachment_.sendInitialUpdate();
}

void GridParameterAttachment::setValue(float newValue)
{
    int div = GdFindNearestDivisor(newValue);
    int index = combo_.indexOfItemId(div);

    if (index == combo_.getSelectedItemIndex())
        return;

    const juce::ScopedValueSetter<bool> svs(ignoreCallbacks_, true);
    combo_.setSelectedItemIndex(index, juce::sendNotificationSync);
}

void GridParameterAttachment::comboBoxChanged(juce::ComboBox *combo)
{
    if (ignoreCallbacks_)
        return;

    int id = combo->getSelectedId();
    attachment_.setValueAsCompleteGesture((float)id);
}
