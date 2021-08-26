// SPDX-License-Identifier: BSD-2-Clause
#include "TapEditScreen.h"
#include "TapSlider.h"
#include "editor/utility/FunctionalTimer.h"
#include <Gd.h>
#include <chrono>
namespace kro = std::chrono;

struct TapEditScreen::Impl : public TapEditItem::Listener,
                             public TapMiniMap::Listener {
    using Listener = TapEditScreen::Listener;

    TapEditScreen *self_ = nullptr;
    juce::ListenerList<Listener> listeners_;

    std::unique_ptr<TapEditItem> items_[GdMaxLines];
    std::unique_ptr<TapMiniMap> miniMap_;
    juce::Range<float> timeRange_{0, 1};
    TapEditMode editMode_ = kTapEditOff;

    int xMargin_ = 25;

    ///
    bool tapHasBegun_ = false;
    unsigned tapCaptureCount_ = 0;
    kro::steady_clock::time_point tapBeginTime_;
    std::unique_ptr<juce::Timer> tapCaptureTimer_;

    ///
    float delayToX(float t) const noexcept;
    float xToDelay(float x) const noexcept;
    float currentTapTime(kro::steady_clock::time_point now = kro::steady_clock::now()) const noexcept;
    int findUnusedTap() const;
    void createNewTap(int tapNumber, float delay);
    void clearAllTaps();
    void beginTapCapture();
    void nextTapCapture();
    void tickTapCapture();
    void endTapCapture();
    void updateItemSizeAndPosition(int itemNumber);
    void updateAllItemSizesAndPositions();

    ///
    void tapEditStarted(TapEditItem *item, GdParameter id) override;
    void tapEditEnded(TapEditItem *item, GdParameter id) override;
    void tapValueChanged(TapEditItem *item, GdParameter id, float value) override;

    ///
    void miniMapRangeChanged(TapMiniMap *, juce::Range<float> range) override;
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

    TapMiniMap *miniMap = new TapMiniMap;
    impl.miniMap_.reset(miniMap);
    miniMap->setTopLeftPosition(40.0f, 40.0f);
    miniMap->setTimeRange(impl.timeRange_, juce::dontSendNotification);
    miniMap->addListener(&impl);
    addAndMakeVisible(miniMap);

    impl.tapCaptureTimer_.reset(FunctionalTimer::create([&impl]() { impl.tickTapCapture(); }));
}

TapEditScreen::~TapEditScreen()
{
    Impl &impl = *impl_;
    impl.miniMap_->addListener(&impl);
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

juce::Range<float> TapEditScreen::getTimeRange() const noexcept
{
    Impl &impl = *impl_;
    return impl.timeRange_;
}

void TapEditScreen::setTimeRange(juce::Range<float> newTimeRange)
{
    Impl &impl = *impl_;
    if (impl.timeRange_ == newTimeRange)
        return;

    impl.timeRange_ = newTimeRange;
    impl.updateAllItemSizesAndPositions();

    impl.miniMap_->setTimeRange(impl.timeRange_, juce::dontSendNotification);
}

float TapEditScreen::getTapValue(GdParameter id) const
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);

    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];
    return item.getTapValue(id);
}

void TapEditScreen::setTapValue(GdParameter id, float value, juce::NotificationType nt)
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);

    Impl &impl = *impl_;
    TapEditItem &item = *impl.items_[tapNumber];
    item.setTapValue(id, value, nt);
}

void TapEditScreen::beginTap()
{
    Impl &impl = *impl_;

    if (!impl.tapHasBegun_)
        impl.beginTapCapture();
    else
        impl.nextTapCapture();

    repaint();
}

void TapEditScreen::endTap()
{
    Impl &impl = *impl_;

    if (!impl.tapHasBegun_)
        return;

    impl.nextTapCapture();
    impl.endTapCapture();

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

void TapEditScreen::Impl::createNewTap(int tapNumber, float delay)
{
    TapEditScreen *self = self_;

    for (int i = 0; i < GdNumPametersPerTap; ++i) {
        GdParameter decomposedId = (GdParameter)(GdFirstParameterOfFirstTap + i);
        GdParameter id = GdRecomposeParameter(decomposedId, tapNumber);

        switch ((int)decomposedId) {
        case GDP_TAP_A_ENABLE:
            self->setTapValue(id, true);
            break;
        case GDP_TAP_A_DELAY:
            self->setTapValue(id, delay);
            break;
        default:
            self->setTapValue(id, GdParameterDefault(id));
            break;
        }
    }
}

void TapEditScreen::Impl::clearAllTaps()
{
    TapEditScreen *self = self_;

    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        GdParameter id = GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber);
        self->setTapValue(id, false);
    }
}

void TapEditScreen::Impl::beginTapCapture()
{
    TapEditScreen *self = self_;
    tapHasBegun_ = true;
    tapCaptureCount_ = 0;
    tapBeginTime_ = kro::steady_clock::now();
    tapCaptureTimer_->startTimerHz(60);
    listeners_.call([self](Listener &l) { l.tappingHasStarted(self); });
}

void TapEditScreen::Impl::nextTapCapture()
{
    float delay = currentTapTime();
    if (delay > (float)GdMaxDelay)
        return;

    if (tapCaptureCount_ == 0)
        clearAllTaps();

    int nextTapNumber = findUnusedTap();
    if (nextTapNumber == -1)
        return;

    createNewTap(nextTapNumber, delay);
    ++tapCaptureCount_;
}

void TapEditScreen::Impl::tickTapCapture()
{
    TapEditScreen *self = self_;

    if (currentTapTime() > (float)GdMaxDelay)
        endTapCapture();

    self->repaint();
}

void TapEditScreen::Impl::endTapCapture()
{
    TapEditScreen *self = self_;
    tapCaptureTimer_->stopTimer();
    tapHasBegun_ = false;
    listeners_.call([self](Listener &l) { l.tappingHasEnded(self); });
}

void TapEditScreen::updateItemSizeAndPosition(int tapNumber)
{
    Impl &impl = *impl_;
    impl.updateItemSizeAndPosition(tapNumber);
}

void TapEditScreen::updateAllItemSizesAndPositions()
{
    Impl &impl = *impl_;
    impl.updateAllItemSizesAndPositions();
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

    switch ((int)impl.editMode_) {
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
    juce::Rectangle<float> rc = self->getLocalBounds().reduced(xMargin_, 0.0f).toFloat();
    juce::Range<float> tr = timeRange_;
    return rc.getX() + rc.getWidth() * ((t - tr.getStart()) / tr.getLength());
}

float TapEditScreen::Impl::xToDelay(float x) const noexcept
{
    TapEditScreen *self = self_;
    juce::Rectangle<float> rc = self->getLocalBounds().reduced(xMargin_, 0.0f).toFloat();
    juce::Range<float> tr = timeRange_;
    return tr.getStart() + tr.getLength() * ((x - rc.getX()) / rc.getWidth());
}

float TapEditScreen::Impl::currentTapTime(kro::steady_clock::time_point now) const noexcept
{
    kro::steady_clock::duration dur = now - tapBeginTime_;
    float secs = kro::duration<float>(dur).count();
    return secs;
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

void TapEditScreen::Impl::updateAllItemSizesAndPositions()
{
    for (int itemNumber = 0; itemNumber < GdMaxLines; ++itemNumber)
        updateItemSizeAndPosition(itemNumber);
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

void TapEditScreen::Impl::miniMapRangeChanged(TapMiniMap *, juce::Range<float> range)
{
    TapEditScreen *self = self_;
    self->setTimeRange(range);
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

float TapEditItem::getTapValue(GdParameter id) const
{
    Impl &impl = *impl_;

    int tapNumber;
    GdParameter decomposedId = GdDecomposeParameter(id, &tapNumber);

    if (impl.itemNumber_ != tapNumber) {
        jassertfalse;
        return 0.0f;
    }

    switch ((int)decomposedId) {
    case GDP_TAP_A_ENABLE:
        return impl.data_.enabled;
    case GDP_TAP_A_DELAY:
        return impl.data_.delay;
    case GDP_TAP_A_LPF_CUTOFF:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
            return (float)slider->getMaxValue();
        goto notfound;
    case GDP_TAP_A_HPF_CUTOFF:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
            return (float)slider->getMinValue();
        goto notfound;
    case GDP_TAP_A_RESONANCE:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditResonance))
            return (float)slider->getValue();
        goto notfound;
    case GDP_TAP_A_TUNE:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditTune))
            return (float)slider->getValue();
        goto notfound;
    case GDP_TAP_A_PAN:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditPan))
            return (float)slider->getValue();
        goto notfound;
    case GDP_TAP_A_LEVEL:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditLevel))
            return (float)slider->getValue();
        goto notfound;
    default: notfound:
        jassertfalse;
        return 0.0f;
    }
}

void TapEditItem::setTapValue(GdParameter id, float value, juce::NotificationType nt)
{
    Impl &impl = *impl_;

    int tapNumber;
    GdParameter decomposedId = GdDecomposeParameter(id, &tapNumber);

    if (impl.itemNumber_ != tapNumber) {
        jassertfalse;
        return;
    }

    switch ((int)decomposedId) {
    case GDP_TAP_A_ENABLE:
    {
        bool enabled = (bool)value;
        if (impl.data_.enabled == enabled)
            return;

        impl.data_.enabled = enabled;

        if (nt != juce::dontSendNotification)
            impl.listeners_.call([this, enabled](Listener &l) { l.tapValueChanged(this, GdRecomposeParameter(GDP_TAP_A_ENABLE, impl_->itemNumber_), enabled); });

        setVisible(enabled);

        TapEditScreen &screen = *impl.screen_;
        setEditMode(enabled ? screen.getEditMode() : kTapEditOff);
        if (enabled)
            screen.updateItemSizeAndPosition(impl.itemNumber_);

        break;
    }
    case GDP_TAP_A_DELAY:
    {
        float delay = value;
        if (impl.data_.delay == delay)
            return;

        impl.data_.delay = delay;

        if (nt != juce::dontSendNotification)
            impl.listeners_.call([this, delay](Listener &l) { l.tapValueChanged(this, GdRecomposeParameter(GDP_TAP_A_DELAY, impl_->itemNumber_), delay); });

        TapEditScreen &screen = *impl.screen_;
        if (impl.data_.enabled)
            screen.updateItemSizeAndPosition(impl.itemNumber_);

        break;
    }
    case GDP_TAP_A_LPF_CUTOFF:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
            slider->setMaxValue(value, nt);
        break;
    case GDP_TAP_A_HPF_CUTOFF:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditCutoff))
            slider->setMinValue(value, nt);
        break;
    case GDP_TAP_A_RESONANCE:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditResonance))
            slider->setValue(value, nt);
        break;
    case GDP_TAP_A_TUNE:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditTune))
            slider->setValue(value, nt);
        break;
    case GDP_TAP_A_PAN:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditPan))
            slider->setValue(value, nt);
        break;
    case GDP_TAP_A_LEVEL:
        if (TapSlider *slider = impl.getSliderForEditMode(kTapEditLevel))
            slider->setValue(value, nt);
        break;
    }
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
                juce::Range<float> timeRange = screen->getTimeRange();
                int x1 = (int)std::floor(screen->getXForDelay(timeRange.getStart()) - halfWidth);
                int x2 = (int)std::ceil(screen->getXForDelay(timeRange.getEnd()) - halfWidth);
                bounds.setX(juce::jlimit(x1, x2, bounds.getX()));
                bounds.setY(previousBounds.getY());
            }
        private:
            TapEditScreen *screen_ = nullptr;
        };
        TapConstrainer constrainer(impl.screen_);
        impl.dragger_.dragComponent(this, e, &constrainer);
        float newDelay = impl.screen_->getDelayForX(getBounds().toFloat().getCentreX());
        newDelay = impl.screen_->getTimeRange().clipValue(newDelay);
        GdParameter id = GdRecomposeParameter(GDP_TAP_A_DELAY, impl.itemNumber_);
        setTapValue(id, newDelay);
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

//------------------------------------------------------------------------------
struct TapMiniMap::Impl {
    TapMiniMap *self_ = nullptr;
    juce::ListenerList<Listener> listeners_;
    juce::Range<float> timeRange_{0, GdMaxDelay};
    juce::Range<float> timeRangeBeforeMove_;

    enum {
        kStatusNormal,
        kStatusMoving,
        kStatusDraggingLeft,
        kStatusDraggingRight,
    };

    enum {
        kResizeGrabMargin = 4,
    };

    int status_ = kStatusNormal;

    void updateCursor(juce::Point<float> position);
    float getXForDelay(float t) const;
    float getDelayForX(float x) const;
    juce::Rectangle<float> getRangeBounds() const;
    juce::Rectangle<float> getLeftResizeBounds() const;
    juce::Rectangle<float> getRightResizeBounds() const;
};

TapMiniMap::TapMiniMap()
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    setSize(200, 20);
}

TapMiniMap::~TapMiniMap()
{
}

void TapMiniMap::setTimeRange(juce::Range<float> timeRange, juce::NotificationType nt)
{
    Impl &impl = *impl_;
    if (impl.timeRange_ == timeRange)
        return;

    impl.timeRange_ = timeRange;
    repaint();

    if (nt != juce::dontSendNotification)
        impl.listeners_.call([this](Listener &l) { l.miniMapRangeChanged(this, impl_->timeRange_); });
}

void TapMiniMap::addListener(Listener *listener)
{
    Impl &impl = *impl_;
    impl.listeners_.add(listener);
}

void TapMiniMap::removeListener(Listener *listener)
{
    Impl &impl = *impl_;
    impl.listeners_.remove(listener);
}

void TapMiniMap::mouseDown(const juce::MouseEvent &event)
{
    Impl &impl = *impl_;
    juce::Point<float> position = event.position;
    int status = impl.status_;

    if (status == Impl::kStatusNormal) {
        if (impl.getLeftResizeBounds().contains(position)) {
            impl.status_ = Impl::kStatusDraggingLeft;
            impl.updateCursor(event.position);
        }
        else if (impl.getRightResizeBounds().contains(position)) {
            impl.status_ = Impl::kStatusDraggingRight;
            impl.updateCursor(event.position);
        }
        else if (impl.getRangeBounds().contains(position)) {
            impl.status_ = Impl::kStatusMoving;
            impl.timeRangeBeforeMove_ = impl.timeRange_;
            impl.updateCursor(event.position);
        }
    }
}

void TapMiniMap::mouseUp(const juce::MouseEvent &event)
{
    Impl &impl = *impl_;
    juce::Point<float> position = event.position;
    int status = impl.status_;

    if (status != Impl::kStatusNormal) {
        impl.status_ = Impl::kStatusNormal;
        impl.updateCursor(position);
    }
}

void TapMiniMap::mouseMove(const juce::MouseEvent &event)
{
    Impl &impl = *impl_;
    juce::Point<float> position = event.position;
    int status = impl.status_;

    if (status == Impl::kStatusNormal)
        impl.updateCursor(position);
}

void TapMiniMap::mouseDrag(const juce::MouseEvent &event)
{
    Impl &impl = *impl_;
    juce::Point<float> position = event.position;
    int status = impl.status_;

    if (status == Impl::kStatusDraggingLeft) {
        juce::Rectangle<float> rangeBounds = impl.getRangeBounds();
        float minT = 0.0f;
        float maxT = std::max(minT, impl.getDelayForX(rangeBounds.getRight()) - 0.5f);
        float newT = juce::jlimit(minT, maxT, impl.getDelayForX(position.getX()));
        if (impl.timeRange_.getStart() != newT) {
            impl.timeRange_.setStart(newT);
            impl.listeners_.call([this](Listener &l) { l.miniMapRangeChanged(this, impl_->timeRange_); });
            repaint();
        }
    }
    else if (status == Impl::kStatusDraggingRight) {
        juce::Rectangle<float> rangeBounds = impl.getRangeBounds();
        float maxT = GdMaxDelay;
        float minT = std::min(maxT, impl.getDelayForX(rangeBounds.getX()) + 0.5f);
        float newT = juce::jlimit(minT, maxT, impl.getDelayForX(position.getX()));
        if (impl.timeRange_.getEnd() != newT) {
            impl.timeRange_.setEnd(newT);
            impl.listeners_.call([this](Listener &l) { l.miniMapRangeChanged(this, impl_->timeRange_); });
            repaint();
        }
    }
    else if (status == Impl::kStatusMoving) {
        float dt = (float)GdMaxDelay * ((position.x - (float)event.getMouseDownX()) / (float)getWidth());
        juce::Range<float> tr = impl.timeRangeBeforeMove_;
        if (dt > 0)
            dt = std::min(dt, (float)GdMaxDelay - tr.getEnd());
        else if (dt < 0)
            dt = std::max(dt, -tr.getStart());
        tr = {tr.getStart() + dt, tr.getEnd() + dt};
        if (impl.timeRange_ != tr) {
            impl.timeRange_ = tr;
            impl.listeners_.call([this](Listener &l) { l.miniMapRangeChanged(this, impl_->timeRange_); });
            repaint();
        }
    }
}

void TapMiniMap::paint(juce::Graphics &g)
{
    juce::Rectangle<int> bounds = getLocalBounds();

    juce::Colour backColour{0x40000000};
    juce::Colour rangeColour{0x60ffffff};
    juce::Colour contourColour{0x40ffffff};

    g.setColour(backColour);
    g.fillRect(bounds);
    g.setColour(contourColour);
    g.drawRect(bounds);

    Impl &impl = *impl_;
    juce::Rectangle<float> rangeBounds = impl.getRangeBounds().reduced(0.0f, 1.0f);
    g.setColour(rangeColour);
    g.fillRect(rangeBounds);
    g.setColour(contourColour);
    g.drawRect(rangeBounds);
}

void TapMiniMap::Impl::updateCursor(juce::Point<float> position)
{
    TapMiniMap *self = self_;

    switch (status_) {
        case kStatusNormal:
        {
            if (getLeftResizeBounds().contains(position) || getRightResizeBounds().contains(position))
                self->setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            else if (getRangeBounds().contains(position))
                self->setMouseCursor(juce::MouseCursor::PointingHandCursor);
            else
                self->setMouseCursor(juce::MouseCursor::NormalCursor);
            break;
        }
        case kStatusMoving:
            self->setMouseCursor(juce::MouseCursor::DraggingHandCursor);
            break;
        case kStatusDraggingLeft:
        case kStatusDraggingRight:
            self->setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            break;
    }
}

float TapMiniMap::Impl::getXForDelay(float t) const
{
    TapMiniMap *self = self_;
    juce::Rectangle<float> rc = self->getLocalBounds().toFloat();
    return rc.getX() + rc.getWidth() * (t / (float)GdMaxDelay);
}

float TapMiniMap::Impl::getDelayForX(float x) const
{
    TapMiniMap *self = self_;
    juce::Rectangle<float> rc = self->getLocalBounds().toFloat();
    return (float)GdMaxDelay * ((x - rc.getX()) / rc.getWidth());
}

juce::Rectangle<float> TapMiniMap::Impl::getRangeBounds() const
{
    TapMiniMap *self = self_;
    juce::Range<float> tr = timeRange_;
    return self->getLocalBounds().toFloat()
        .withLeft(getXForDelay(tr.getStart()))
        .withRight(getXForDelay(tr.getEnd()));
}

juce::Rectangle<float> TapMiniMap::Impl::getLeftResizeBounds() const
{
    juce::Rectangle<float> rangeBounds = getRangeBounds();
    juce::Rectangle<float> boundsRszL{rangeBounds.getX(), rangeBounds.getY(), 0.0f, rangeBounds.getHeight()};
    boundsRszL.expand(kResizeGrabMargin, 0.0f);
    return boundsRszL;
}

juce::Rectangle<float> TapMiniMap::Impl::getRightResizeBounds() const
{
    juce::Rectangle<float> rangeBounds = getRangeBounds();
    juce::Rectangle<float> boundsRszR{rangeBounds.getRight(), rangeBounds.getY(), 0.0f, rangeBounds.getHeight()};
    boundsRszR.expand(kResizeGrabMargin, 0.0f);
    return boundsRszR;
}
