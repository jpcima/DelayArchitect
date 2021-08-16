// SPDX-License-Identifier: BSD-2-Clause
#include "TapEditScreen.h"
#include "TapSlider.h"
#include "editor/utility/FunctionalTimer.h"
#include <Gd.h>
#include <chrono>
namespace kro = std::chrono;

struct TapEditScreen::Impl : public TapEditItem::Listener {
    using Listener = TapEditScreen::Listener;

    TapEditScreen *self_ = nullptr;
    juce::ListenerList<Listener> listeners_;

    std::unique_ptr<TapEditItem> items_[GdMaxLines];
    float timeRange_ = 10;
    TapEditMode editMode_ = kTapEditOff;

    int xMargin_ = 25;

    ///
    bool tapHasBegun_ = false;
    kro::steady_clock::time_point tapBeginTime_;
    std::unique_ptr<juce::Timer> tapRedisplayTimer_;

    ///
    float delayToX(float t) const noexcept;
    float xToDelay(float x) const noexcept;
    float currentTapTime(kro::steady_clock::time_point now = kro::steady_clock::now()) const noexcept;
    int findUnusedTap() const;
    void updateItemSizeAndPosition(int itemNumber);

    ///
    void tapEditStarted(TapEditItem *item, GdParameter id) override;
    void tapEditEnded(TapEditItem *item, GdParameter id) override;
    void tapValueChanged(TapEditItem *item, GdParameter id, float value) override;
};

TapEditScreen::TapEditScreen()
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    for (int itemNumber = 0; itemNumber < GdMaxLines; ++itemNumber) {
        TapEditItem *item = new TapEditItem(this, itemNumber);
        impl.items_[itemNumber].reset(item);
        item->addListener(&impl);
        addChildComponent(item);
        impl.updateItemSizeAndPosition(itemNumber);
    }

    impl.tapRedisplayTimer_.reset(FunctionalTimer::create([this]() {
        repaint();
    }));
}

TapEditScreen::~TapEditScreen()
{
}

TapEditMode TapEditScreen::getEditMode() const noexcept
{
    Impl &impl = *impl_;
    return impl.editMode_;
}

void TapEditScreen::setEditMode(TapEditMode mode)
{
    Impl &impl = *impl_;
    if (impl.editMode_ == mode)
        return;

    impl.editMode_ = mode;
    for (int itemNumber = 0; itemNumber < GdMaxLines; ++itemNumber) {
        TapEditItem &item = *impl.items_[itemNumber];
        const TapEditData &data = item.getData();
        item.setEditMode(data.enabled ? mode : kTapEditOff);
    }

    repaint();
}

float TapEditScreen::getTimeRange() const noexcept
{
    Impl &impl = *impl_;
    return impl.timeRange_;
}

void TapEditScreen::setTimeRange(float maxTime)
{
    Impl &impl = *impl_;
    if (impl.timeRange_ == maxTime)
        return;

    impl.timeRange_ = maxTime;
    for (int itemNumber = 0; itemNumber < GdMaxLines; ++itemNumber)
        impl.updateItemSizeAndPosition(itemNumber);
}

void TapEditScreen::setTapEnabled(int tapNumber, bool enabled, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapEnabled(enabled, nt);
    item.setEditMode(enabled ? impl.editMode_ : kTapEditOff);

    if (enabled)
        impl.updateItemSizeAndPosition(tapNumber);
}

void TapEditScreen::setTapDelay(int tapNumber, float delay, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapDelay(delay, nt);
    impl.updateItemSizeAndPosition(tapNumber);
}

void TapEditScreen::setTapLPFCutoff(int tapNumber, float cutoff, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapLPFCutoff(cutoff, nt);
}

void TapEditScreen::setTapHPFCutoff(int tapNumber, float cutoff, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapHPFCutoff(cutoff, nt);
}

void TapEditScreen::setTapResonance(int tapNumber, float resonance, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapResonance(resonance, nt);
}

void TapEditScreen::setTapTune(int tapNumber, float tune, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapTune(tune, nt);
}

void TapEditScreen::setTapPan(int tapNumber, float pan, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapPan(pan, nt);
}

void TapEditScreen::setTapLevel(int tapNumber, float level, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];

    item.setTapLevel(level, nt);
}

void TapEditScreen::beginTap()
{
    Impl &impl = *impl_;

    if (!impl.tapHasBegun_) {
        impl.tapHasBegun_ = true;
        impl.tapBeginTime_ = kro::steady_clock::now();
        impl.tapRedisplayTimer_->startTimerHz(60);
        impl.listeners_.call([this](Listener &l) { l.tappingHasStarted(this); });
    }
    else {
        int nextTapNumber = impl.findUnusedTap();
        if (nextTapNumber != -1) {
            float delay = impl.currentTapTime();
            setTapEnabled(nextTapNumber, true);
            setTapDelay(nextTapNumber, delay);
        }
    }
}

void TapEditScreen::endTap()
{
    Impl &impl = *impl_;

    if (!impl.tapHasBegun_)
        return;

    int nextTapNumber = impl.findUnusedTap();
    if (nextTapNumber != -1) {
        float delay = impl.currentTapTime();
        setTapEnabled(nextTapNumber, true);
        setTapDelay(nextTapNumber, delay);
    }

    impl.tapRedisplayTimer_->stopTimer();
    impl.tapHasBegun_ = false;
    impl.listeners_.call([this](Listener &l) { l.tappingHasEnded(this); });

    repaint();
}

int TapEditScreen::Impl::findUnusedTap() const
{
    int selectedNumber = -1;

    for (int itemNumber = 0; itemNumber < GdMaxLines && selectedNumber == -1; ++itemNumber) {
        TapEditItem &item = *items_[itemNumber];
        const TapEditData &data = item.getData();
        if (!data.enabled)
            selectedNumber = itemNumber;
    }

    return selectedNumber;
}

void TapEditScreen::updateItemSizeAndPosition(int tapNumber)
{
    Impl &impl = *impl_;
    impl.updateItemSizeAndPosition(tapNumber);
}

float TapEditScreen::getXForDelay(float delay) const
{
    Impl &impl = *impl_;
    return impl.delayToX(delay);
}

float TapEditScreen::getDelayForX(float x) const
{
    Impl &impl = *impl_;
    return impl.xToDelay(x);
}

void TapEditScreen::addListener(Listener *listener)
{
    Impl &impl = *impl_;
    impl.listeners_.add(listener);
}

void TapEditScreen::removeListener(Listener *listener)
{
    Impl &impl = *impl_;
    impl.listeners_.remove(listener);
}

void TapEditScreen::paint(juce::Graphics &g)
{
    juce::Component::paint(g);

    Impl &impl = *impl_;
    juce::Rectangle<int> bounds = getLocalBounds();

    switch (impl.editMode_) {
    case kTapEditTune:
    case kTapEditPan:
        {
            juce::Colour lineColour = findColour(lineColourId);
            g.setColour(lineColour);
            float lineY = 0.5f * (float)(bounds.getHeight() - impl.items_[0]->getLabelHeight());
            g.drawHorizontalLine((int)(lineY + 0.5f), (float)bounds.getX(), (float)bounds.getRight());
            break;
        }
    default:
        break;
    }

    if (impl.tapHasBegun_) {
        juce::Colour tapLineColour = findColour(tapLineColourId);
        float tapLineX = impl.delayToX(impl.currentTapTime());

        g.setColour(tapLineColour);
        g.drawLine(tapLineX, 0.0f, tapLineX, (float)bounds.getBottom());
    }
}

float TapEditScreen::Impl::delayToX(float t) const noexcept
{
    TapEditScreen *self = self_;
    float tr = timeRange_;
    int width = self->getWidth();
    int xm = xMargin_;
    return (float)xm + t * (float)(width - 2 * xm) / tr;
}

float TapEditScreen::Impl::xToDelay(float x) const noexcept
{
    TapEditScreen *self = self_;
    float tr = timeRange_;
    int width = self->getWidth();
    int xm = xMargin_;
    return (x - (float)xm) * tr / (float)(width - 2 * xm);
}

float TapEditScreen::Impl::currentTapTime(kro::steady_clock::time_point now) const noexcept
{
    kro::steady_clock::duration dur = now - tapBeginTime_;
    float secs = kro::duration<float>(dur).count();
    float tr = timeRange_;
    return std::fmod(secs, tr);
}

void TapEditScreen::Impl::updateItemSizeAndPosition(int itemNumber)
{
    TapEditScreen *self = self_;
    juce::Rectangle<int> bounds = self->getLocalBounds();
    TapEditItem &item = *items_[itemNumber];
    const TapEditData &data = item.getData();
    int width = item.getLabelWidth();
    int height = bounds.getHeight();
    item.setSize(width, height);
    item.setTopLeftPosition((int)(delayToX(data.delay) - 0.5f * (float)width), 0);
}

void TapEditScreen::Impl::tapEditStarted(TapEditItem *, GdParameter id)
{
    TapEditScreen *self = self_;
    listeners_.call([self, id](Listener &listener) { listener.tapEditStarted(self, id); });
}

void TapEditScreen::Impl::tapEditEnded(TapEditItem *, GdParameter id)
{
    TapEditScreen *self = self_;
    listeners_.call([self, id](Listener &listener) { listener.tapEditEnded(self, id); });
}

void TapEditScreen::Impl::tapValueChanged(TapEditItem *, GdParameter id, float value)
{
    TapEditScreen *self = self_;
    listeners_.call([self, id, value](Listener &listener) { listener.tapValueChanged(self, id, value); });
}

//------------------------------------------------------------------------------
struct TapEditItem::Impl : public TapSlider::Listener {
    using Listener = TapEditItem::Listener;

    TapEditItem *self_ = nullptr;
    juce::ListenerList<Listener> listeners_;
    juce::ComponentDragger dragger_;
    GdParameter dragChangeId_ = GDP_NONE;
    TapEditData data_;
    TapEditScreen *screen_ = nullptr;
    int itemNumber_ {};
    int labelWidth_ = 20;
    int labelHeight_ = 20;
    TapEditMode editMode_ = kTapEditOff;
    std::map<TapEditMode, std::unique_ptr<TapSlider>> sliders_;

    TapSlider *getCurrentSlider() const;
    TapSlider *getSliderForEditMode(TapEditMode editMode) const;
    void updateSliderVisibility();
    void repositionSliders();

    void sliderValueChanged(juce::Slider *slider) override;
    void sliderDragStarted(juce::Slider *slider) override;
    void sliderDragEnded(juce::Slider *slider) override;
};

TapEditItem::TapEditItem(TapEditScreen *screen, int itemNumber)
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;
    impl.itemNumber_ = itemNumber;
    impl.screen_ = screen;

    enum TapSliderKind {
        kTapSliderNormal,
        kTapSliderBipolar,
        kTapSliderTwoValues,
    };

    auto createSlider = [this, &impl]
        (TapEditMode mode, GdParameter id, GdParameter id2, int kind)
    {
        TapSlider *slider = new TapSlider;
        impl.sliders_[mode] = std::unique_ptr<TapSlider>(slider);
        float min = GdParameterMin((GdParameter)id);
        float max = GdParameterMax((GdParameter)id);
        float def = GdParameterDefault((GdParameter)id);
        slider->setRange(min, max);
        slider->setValue(def);
        slider->setDoubleClickReturnValue(true, def);
        if (kind == kTapSliderBipolar)
            slider->setBipolarAround(true, def);
        else if (kind == kTapSliderTwoValues)
            slider->setSliderStyle(juce::Slider::TwoValueVertical);
        slider->addListener(&impl);
        juce::NamedValueSet &properties = slider->getProperties();
        if (kind != kTapSliderTwoValues)
            properties.set("X-Change-ID", (int)id);
        else {
            properties.set("X-Change-ID-1", (int)id);
            properties.set("X-Change-ID-2", (int)id2);
        }
        addChildComponent(slider);
    };

    createSlider(kTapEditCutoff, GdRecomposeParameter(GDP_TAP_A_HPF_CUTOFF, itemNumber), GdRecomposeParameter(GDP_TAP_A_LPF_CUTOFF, itemNumber), kTapSliderTwoValues);
    createSlider(kTapEditResonance, GdRecomposeParameter(GDP_TAP_A_RESONANCE, itemNumber), GDP_NONE, kTapSliderNormal);
    createSlider(kTapEditTune, GdRecomposeParameter(GDP_TAP_A_TUNE, itemNumber), GDP_NONE, kTapSliderBipolar);
    createSlider(kTapEditPan, GdRecomposeParameter(GDP_TAP_A_PAN, itemNumber), GDP_NONE, kTapSliderBipolar);
    createSlider(kTapEditLevel, GdRecomposeParameter(GDP_TAP_A_LEVEL, itemNumber), GDP_NONE, kTapSliderNormal);

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
        slider->setSkewFactor(0.25f);
}

TapEditItem::~TapEditItem()
{
}

int TapEditItem::getItemNumber() const noexcept
{
    Impl &impl = *impl_;
    return impl.itemNumber_;
}

const TapEditData &TapEditItem::getData() const noexcept
{
    Impl &impl = *impl_;
    return impl.data_;
}

int TapEditItem::getLabelWidth() const noexcept
{
    Impl &impl = *impl_;
    return impl.labelWidth_;
}

int TapEditItem::getLabelHeight() const noexcept
{
    Impl &impl = *impl_;
    return impl.labelHeight_;
}

TapEditMode TapEditItem::getEditMode() const noexcept
{
    Impl &impl = *impl_;
    return impl.editMode_;
}

void TapEditItem::setEditMode(TapEditMode mode)
{
    Impl &impl = *impl_;
    if (impl.editMode_ == mode)
        return;

    impl.editMode_ = mode;

    impl.updateSliderVisibility();

    repaint();
}

void TapEditItem::setTapEnabled(bool enabled, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    if (impl.data_.enabled == enabled)
        return;

    impl.data_.enabled = enabled;

    if (nt != juce::dontSendNotification)
        impl.listeners_.call([this, enabled](Listener &l) { l.tapValueChanged(this, GdRecomposeParameter(GDP_TAP_A_ENABLE, impl_->itemNumber_), enabled); });

    setVisible(enabled);
}

void TapEditItem::setTapDelay(float delay, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    if (impl.data_.delay == delay)
        return;

    impl.data_.delay = delay;

    if (nt != juce::dontSendNotification)
        impl.listeners_.call([this, delay](Listener &l) { l.tapValueChanged(this, GdRecomposeParameter(GDP_TAP_A_DELAY, impl_->itemNumber_), delay); });

    impl.screen_->updateItemSizeAndPosition(impl.itemNumber_);
}

void TapEditItem::setTapLPFCutoff(float cutoff, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
        slider->setMaxValue(cutoff, nt);
}

void TapEditItem::setTapHPFCutoff(float cutoff, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
        slider->setMinValue(cutoff, nt);
}

void TapEditItem::setTapResonance(float resonance, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditResonance))
        slider->setValue(resonance, nt);
}

void TapEditItem::setTapTune(float tune, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditTune))
        slider->setValue(tune, nt);
}

void TapEditItem::setTapPan(float pan, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditPan))
        slider->setValue(pan, nt);
}

void TapEditItem::setTapLevel(float level, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    if (TapSlider *slider = impl.getSliderForEditMode(kTapEditLevel))
        slider->setValue(level, nt);
}

void TapEditItem::addListener(Listener *listener)
{
    Impl &impl = *impl_;
    impl.listeners_.add(listener);
}

void TapEditItem::removeListener(Listener *listener)
{
    Impl &impl = *impl_;
    impl.listeners_.remove(listener);
}

void TapEditItem::paint(juce::Graphics &g)
{
    juce::Component::paint(g);

    Impl &impl = *impl_;
    juce::Rectangle<int> bounds = getLocalBounds();
    juce::Colour lineColour = findColour(TapEditScreen::lineColourId);

    juce::Rectangle<int> rectTemp(bounds);
    juce::Rectangle<int> labelBounds = rectTemp.removeFromBottom(impl.labelHeight_);

    char labelTextCstr[2];
    labelTextCstr[0] = (char)(impl.itemNumber_ + 'A');
    labelTextCstr[1] = '\0';

    g.setColour(lineColour);
    g.drawRect(labelBounds);
    g.drawText(labelTextCstr, labelBounds, juce::Justification::centred);
}

void TapEditItem::mouseDown(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;
    juce::Rectangle<int> bounds = getLocalBounds();

    if (impl.dragChangeId_ == GDP_NONE && e.y >= bounds.getBottom() - impl.labelHeight_) {
        impl.dragChangeId_ = GdRecomposeParameter(GDP_TAP_A_DELAY, impl.itemNumber_);
        impl.dragger_.startDraggingComponent(this, e);
        impl.listeners_.call([this](Listener &l) { l.tapEditStarted(this, impl_->dragChangeId_); });
        return;
    }

    juce::Component::mouseDown(e);
}

void TapEditItem::mouseUp(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;

    if (impl.dragChangeId_ != GDP_NONE) {
        impl.listeners_.call([this](Listener &l) { l.tapEditEnded(this, impl_->dragChangeId_); });
        impl.dragChangeId_ = GDP_NONE;
        return;
    }

    juce::Component::mouseUp(e);
}

void TapEditItem::mouseDrag(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;

    if (impl.dragChangeId_ != GDP_NONE) {
        class TapConstrainer : public juce::ComponentBoundsConstrainer {
        public:
            explicit TapConstrainer(TapEditScreen *screen)
                : screen_(screen)
            {
            }
            void checkBounds(juce::Rectangle<int> &bounds, const juce::Rectangle<int> &previousBounds, const juce::Rectangle<int> &, bool, bool, bool, bool) override
            {
                TapEditScreen *screen = screen_;
                float halfWidth = 0.5f * (float)bounds.getWidth();
                int x1 = (int)std::floor(screen->getXForDelay(0.0f) - halfWidth);
                int x2 = (int)std::ceil(screen->getXForDelay(screen->getTimeRange()) - halfWidth);
                bounds.setX(juce::jlimit(x1, x2, bounds.getX()));
                bounds.setY(previousBounds.getY());
            }
        private:
            TapEditScreen *screen_ = nullptr;
        };
        TapConstrainer constrainer(impl.screen_);
        impl.dragger_.dragComponent(this, e, &constrainer);
        float newDelay = impl.screen_->getDelayForX(getBounds().toFloat().getCentreX());
        newDelay = juce::jlimit(0.0f, impl.screen_->getTimeRange(), newDelay);
        setTapDelay(newDelay);
        return;
    }

    juce::Component::mouseDrag(e);
}

void TapEditItem::moved()
{
    Impl &impl = *impl_;
    impl.repositionSliders();
}

void TapEditItem::resized()
{
    Impl &impl = *impl_;
    impl.repositionSliders();
}

TapSlider *TapEditItem::Impl::getCurrentSlider() const
{
    return getSliderForEditMode(editMode_);
}

TapSlider *TapEditItem::Impl::getSliderForEditMode(TapEditMode editMode) const
{
    auto it = sliders_.find(editMode);
    return (it == sliders_.end()) ? nullptr : it->second.get();
}

void TapEditItem::Impl::updateSliderVisibility()
{
    TapSlider *currentSlider = getCurrentSlider();
    for (const auto &sliderPair : sliders_) {
        TapSlider *slider = sliderPair.second.get();
        slider->setVisible(slider == currentSlider);
    }
}

void TapEditItem::Impl::repositionSliders()
{
    TapEditItem *self = self_;
    juce::Rectangle<int> bounds = self->getLocalBounds();
    juce::Rectangle<int> sliderBounds = bounds.withTrimmedBottom(labelHeight_);
    sliderBounds = sliderBounds.withSizeKeepingCentre(8, sliderBounds.getHeight());

    for (const auto &sliderPair : sliders_) {
        TapSlider *slider = sliderPair.second.get();
        slider->setBounds(sliderBounds);
    }
}

void TapEditItem::Impl::sliderValueChanged(juce::Slider *slider)
{
    double value{};
    juce::String identifier;
    switch (slider->getThumbBeingDragged()) {
    default:
        value = slider->getValue();
        identifier = "X-Change-ID";
        break;
    case 1:
        value = slider->getMinValue();
        identifier = "X-Change-ID-1";
        break;
    case 2:
        value = slider->getMaxValue();
        identifier = "X-Change-ID-2";
        break;
    }

    TapEditItem *self = self_;
    GdParameter id = (GdParameter)(int)slider->getProperties().getWithDefault(identifier, -1);
    if (id != GDP_NONE)
        listeners_.call([self, id, value](Listener &l) { l.tapValueChanged(self, id, (float)value); });
}

void TapEditItem::Impl::sliderDragStarted(juce::Slider *slider)
{
    juce::String identifier;
    switch (slider->getThumbBeingDragged()) {
    default:
        identifier = "X-Change-ID";
        break;
    case 1:
        identifier = "X-Change-ID-1";
        break;
    case 2:
        identifier = "X-Change-ID-2";
        break;
    }

    TapEditItem *self = self_;
    GdParameter id = (GdParameter)(int)slider->getProperties().getWithDefault(identifier, -1);
    if (id != GDP_NONE)
        listeners_.call([self, id](Listener &l) { l.tapEditStarted(self, id); });
}

void TapEditItem::Impl::sliderDragEnded(juce::Slider *slider)
{
    juce::String identifier;
    switch (slider->getThumbBeingDragged()) {
    default:
        identifier = "X-Change-ID";
        break;
    case 1:
        identifier = "X-Change-ID-1";
        break;
    case 2:
        identifier = "X-Change-ID-2";
        break;
    }

    TapEditItem *self = self_;
    GdParameter id = (GdParameter)(int)slider->getProperties().getWithDefault(identifier, -1);
    if (id != GDP_NONE)
        listeners_.call([self, id](Listener &l) { l.tapEditEnded(self, id); });
}
