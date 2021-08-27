// SPDX-License-Identifier: BSD-2-Clause
#include "TapEditScreen.h"
#include "TapSlider.h"
#include "editor/utility/FunctionalTimer.h"
#include <Gd.h>
#include <chrono>
namespace kro = std::chrono;

struct TapEditScreen::Impl : public TapEditItem::Listener,
                             public TapMiniMap::Listener,
                             public juce::ChangeListener {
    using Listener = TapEditScreen::Listener;

    TapEditScreen *self_ = nullptr;
    juce::ListenerList<Listener> listeners_;

    ///
    std::unique_ptr<TapEditItem> items_[GdMaxLines];
    std::unique_ptr<TapMiniMap> miniMap_;
    juce::Range<float> timeRange_{0, 1};
    TapEditMode editMode_ = kTapEditOff;

    bool sync_ = true;
    int div_ = GdDefaultDivisor;
    float swing_ = 0.5f;
    double bpm_ = 120.0;

    enum {
        xMargin = 10,
        yMargin = 10,
    };

    ///
    bool tapHasBegun_ = false;
    unsigned tapCaptureCount_ = 0;
    kro::steady_clock::time_point tapBeginTime_;
    std::unique_ptr<juce::Timer> tapCaptureTimer_;

    ///
    class TapLassoSource : public juce::LassoSource<TapEditItem *> {
    public:
        explicit TapLassoSource(Impl &impl);
        void findLassoItemsInArea(juce::Array<TapEditItem *> &itemsFound, const juce::Rectangle<int> &area) override;
        juce::SelectedItemSet<TapEditItem *> &getLassoSelection() override;
    private:
        Impl *impl_ = nullptr;
    };
    using TapLassoComponent = juce::LassoComponent<TapEditItem *>;
    std::unique_ptr<TapLassoComponent> lasso_;
    std::unique_ptr<TapLassoSource> lassoSource_;
    juce::SelectedItemSet<TapEditItem *> lassoSelection_;

    ///
    enum {
        kStatusNormal,
        kStatusClicked,
        kStatusLasso,
    };
    int status_ = kStatusNormal;

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
    void autoZoomTimeRange();
    void updateItemSizeAndPosition(int itemNumber);
    void updateAllItemSizesAndPositions();

    ///
    void tapEditStarted(TapEditItem *item, GdParameter id) override;
    void tapEditEnded(TapEditItem *item, GdParameter id) override;
    void tapValueChanged(TapEditItem *item, GdParameter id, float value) override;

    ///
    void miniMapRangeChanged(TapMiniMap *, juce::Range<float> range) override;

    ///
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
};

TapEditScreen::TapEditScreen()
    : impl_(new Impl)
{
    Impl &impl = *impl_;
    impl.self_ = this;

    setWantsKeyboardFocus(true);

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

    Impl::TapLassoComponent *lasso = new Impl::TapLassoComponent;
    impl.lasso_.reset(lasso);
    addChildComponent(lasso);
    lasso->setColour(Impl::TapLassoComponent::lassoFillColourId, findColour(lassoFillColourId));
    lasso->setColour(Impl::TapLassoComponent::lassoOutlineColourId, findColour(lassoOutlineColourId));
    Impl::TapLassoSource *lassoSource = new Impl::TapLassoSource(impl);
    impl.lassoSource_.reset(lassoSource);
    impl.lassoSelection_.addChangeListener(&impl);

    impl.tapCaptureTimer_.reset(FunctionalTimer::create([&impl]() { impl.tickTapCapture(); }));
}

TapEditScreen::~TapEditScreen()
{
    Impl &impl = *impl_;
    impl.miniMap_->removeListener(&impl);
    impl.lassoSelection_.removeChangeListener(&impl);
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

    repaint();
}

float TapEditScreen::getTapValue(GdParameter id) const
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);

    Impl &impl = *impl_;

    switch ((int)id) {
    default:
        if (tapNumber != -1) {
            TapEditItem &item = *impl.items_[tapNumber];
            return item.getTapValue(id);
        }
        return 0;
    case GDP_SYNC:
        return impl.sync_;
    case GDP_GRID:
        return (float)impl.div_;
    case GDP_SWING:
        return impl.swing_ * 100.0f;
    }
}

void TapEditScreen::setTapValue(GdParameter id, float value, juce::NotificationType nt)
{
    int tapNumber;
    GdDecomposeParameter(id, &tapNumber);

    Impl &impl = *impl_;

    switch ((int)id) {
    default:
        if (tapNumber != -1) {
            TapEditItem &item = *impl.items_[tapNumber];
            item.setTapValue(id, value, nt);
        }
        break;
    case GDP_SYNC:
        impl.sync_ = (bool)value;
        updateAllItemSizesAndPositions();
        repaint();
        break;
    case GDP_GRID:
        impl.div_ = GdFindNearestDivisor(value);
        updateAllItemSizesAndPositions();
        repaint();
        break;
    case GDP_SWING:
        impl.swing_ = value / 100.0f;
        updateAllItemSizesAndPositions();
        repaint();
        break;
    }
}

bool TapEditScreen::isTapSelected(int tapNumber) const
{
    Impl &impl = *impl_;
    return impl.items_[tapNumber]->isTapSelected();
}

void TapEditScreen::setAllTapsSelected(bool selected)
{
    Impl &impl = *impl_;
    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        TapEditItem &item = *impl.items_[tapNumber];
        item.setTapSelected(selected);
    }
}

void TapEditScreen::setOnlyTapSelected(int selectedTapNumber)
{
    Impl &impl = *impl_;
    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        TapEditItem &item = *impl.items_[tapNumber];
        item.setTapSelected(tapNumber == selectedTapNumber);
    }
}

double TapEditScreen::getBPM() const
{
    Impl &impl = *impl_;
    return impl.bpm_;
}

void TapEditScreen::setBPM(double bpm)
{
    Impl &impl = *impl_;
    if (impl.bpm_ == bpm)
        return;

    impl.bpm_ = bpm;
    repaint();
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
    self->setTimeRange({0, GdMaxDelay});
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

    TapEditScreen *self = self_;
    delay = self->alignDelayToGrid(delay);

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

    autoZoomTimeRange();
}

void TapEditScreen::Impl::autoZoomTimeRange()
{
    int count = 0;
    float maxDelay = 0;

    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        TapEditItem &item = *items_[tapNumber];

        bool enable = (bool)item.getTapValue(GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber));
        float delay = item.getTapValue(GdRecomposeParameter(GDP_TAP_A_DELAY, tapNumber));

        if (enable) {
            maxDelay = std::max(delay, maxDelay);
            ++count;
        }
    }

    if (count == 0)
        maxDelay = GdMaxDelay;
    else
    {
        const float interval = 0.5f;
        maxDelay = std::min((float)GdMaxDelay, interval * std::floor((maxDelay + interval) / interval));
    }

    TapEditScreen *self = self_;
    self->setTimeRange({0, maxDelay});
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

float TapEditScreen::alignDelayToGrid(float delay) const
{
    Impl &impl = *impl_;
    float newDelay;
    if (!impl.sync_)
        newDelay = juce::jlimit(0.0f, (float)GdMaxDelay, delay);
    else
        newDelay = GdAlignDelayToGrid(delay, impl.div_, impl.swing_, (float)impl.bpm_);
    return newDelay;
}

juce::Rectangle<int> TapEditScreen::getLocalBoundsNoMargin() const
{
    return getLocalBounds().reduced(Impl::xMargin, Impl::yMargin);
}

juce::Rectangle<int> TapEditScreen::getScreenBounds() const
{
    return getIntervalsRow().getUnion(getSlidersRow());
}

juce::Rectangle<int> TapEditScreen::getIntervalsRow() const
{
    int intervalsHeight = TapEditItem::getLabelHeight();
    return getLocalBoundsNoMargin().removeFromBottom(intervalsHeight);
}

juce::Rectangle<int> TapEditScreen::getSlidersRow() const
{
    int intervalsHeight = TapEditItem::getLabelHeight();
    return getLocalBoundsNoMargin().withTrimmedBottom(intervalsHeight);
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
    juce::Rectangle<int> screenBounds = getScreenBounds();
    juce::Rectangle<int> intervalsRow = getIntervalsRow();
    juce::Rectangle<int> slidersRow = getSlidersRow();

    juce::Colour screenContourColour = findColour(screenContourColourId);
    juce::Colour intervalFillColour = findColour(intervalFillColourId);
    juce::Colour intervalContourColour = findColour(intervalContourColourId);
    juce::Colour minorIntervalTickColour = findColour(minorIntervalTickColourId);
    juce::Colour majorIntervalTickColour = findColour(majorIntervalTickColourId);
    juce::Colour superMajorIntervalTickColour = findColour(superMajorIntervalTickColourId);

    g.setColour(screenContourColour);
    g.drawRect(screenBounds);
    g.setColour(intervalFillColour);
    g.fillRect(intervalsRow);
    if (impl.sync_) {
        int div = impl.div_;
        int majorDiv = div / ((div & 3) ? 2 : 4);
        int superMajorDiv = div;
        float swing = impl.swing_;
        float bpm = (float)impl.bpm_;
        for (int i = 0; ; ++i) {
            float d = GdGetGridTick(i, div, swing, bpm);
            float x = getXForDelay(d);
            if (x > (float)intervalsRow.getRight()) break;
            g.setColour((i % superMajorDiv == 0) ? superMajorIntervalTickColour :
                        (i % majorDiv == 0) ? majorIntervalTickColour :
                        minorIntervalTickColour);
            g.drawLine(x, (float)(intervalsRow.getY() + 1), x, (float)(intervalsRow.getBottom() - 1));
            if (d >= (float)GdMaxDelay) break;
        }
    }
    g.setColour(intervalContourColour);
    g.drawRect(intervalsRow);

    switch ((int)impl.editMode_) {
    case kTapEditTune:
    case kTapEditPan:
    {
        juce::Colour lineColour = findColour(lineColourId);
        g.setColour(lineColour);
        float lineY = slidersRow.toFloat().getCentreY();
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

void TapEditScreen::mouseDown(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;
    juce::Rectangle<int> intervalsRow = getIntervalsRow();

    if (intervalsRow.toFloat().contains(e.position)) {
        float delay = alignDelayToGrid(getDelayForX(e.position.getX()));
        int tapNumber = impl.findUnusedTap();
        if (tapNumber != -1) {
            impl.createNewTap(tapNumber, delay);
            setOnlyTapSelected(tapNumber);
        }
    }
    else {
        impl.status_ = Impl::kStatusClicked;
    }
}

void TapEditScreen::mouseUp(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;

    (void)e;

    switch (impl.status_) {
    case Impl::kStatusClicked:
        setAllTapsSelected(false);
        impl.status_ = Impl::kStatusNormal;
        break;
    case Impl::kStatusLasso:
        impl.lasso_->endLasso();
        impl.status_ = Impl::kStatusNormal;
        break;
    }
}

void TapEditScreen::mouseDrag(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;

    switch (impl.status_) {
    case Impl::kStatusClicked:
        impl.lasso_->beginLasso(e, impl.lassoSource_.get());
        impl.status_ = Impl::kStatusLasso;
        break;
    case Impl::kStatusLasso:
        impl.lasso_->dragLasso(e);
        break;
    }
}

bool TapEditScreen::keyPressed(const juce::KeyPress &e)
{
    if (e.isKeyCode(juce::KeyPress::deleteKey)) {
        for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
            if (isTapSelected(tapNumber)) {
                GdParameter id = GdRecomposeParameter(GDP_TAP_A_ENABLE, tapNumber);
                setTapValue(id, false);
            }
        }
        setAllTapsSelected(false);
    }
    return false;
}

float TapEditScreen::Impl::delayToX(float t) const noexcept
{
    TapEditScreen *self = self_;
    juce::Rectangle<float> rc = self->getLocalBoundsNoMargin().toFloat().reduced((float)TapEditItem::getLabelWidth() / 2.0f, 0);
    juce::Range<float> tr = timeRange_;
    return rc.getX() + rc.getWidth() * ((t - tr.getStart()) / tr.getLength());
}

float TapEditScreen::Impl::xToDelay(float x) const noexcept
{
    TapEditScreen *self = self_;
    juce::Rectangle<float> rc = self->getLocalBoundsNoMargin().toFloat().reduced((float)TapEditItem::getLabelWidth() / 2.0f, 0);
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
    juce::Rectangle<int> screenBounds = self->getScreenBounds();
    TapEditItem &item = *items_[itemNumber];
    const TapEditData &data = item.getData();
    int width = item.getLabelWidth();
    int height = screenBounds.getHeight();
    item.setSize(width, height);
    item.setTopLeftPosition((int)(delayToX(data.delay) - 0.5f * (float)width), screenBounds.getY());
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

void TapEditScreen::Impl::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (source == &lassoSelection_) {
        bool selected[GdMaxLines] = {};
        for (TapEditItem *item : lassoSelection_)
            selected[item->getItemNumber()] = true;

        for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
            TapEditItem &item = *items_[tapNumber];
            item.setTapSelected(selected[tapNumber]);
        }
    }
}

///
TapEditScreen::Impl::TapLassoSource::TapLassoSource(Impl &impl)
    : impl_(&impl)
{
}

void TapEditScreen::Impl::TapLassoSource::findLassoItemsInArea(juce::Array<TapEditItem *> &itemsFound, const juce::Rectangle<int> &area)
{
    Impl &impl = *impl_;
    for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
        TapEditItem &item = *impl.items_[tapNumber];
        if (item.isVisible() && area.intersects(item.getBounds()))
            itemsFound.add(&item);
    }
}

juce::SelectedItemSet<TapEditItem *> &TapEditScreen::Impl::TapLassoSource::getLassoSelection()
{
    Impl &impl = *impl_;
    return impl.lassoSelection_;
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
    TapEditMode editMode_ = kTapEditOff;
    std::map<TapEditMode, std::unique_ptr<TapSlider>> sliders_;
    bool tapSelected_ = false;

    TapSlider *getCurrentSlider() const;
    TapSlider *getSliderForEditMode(TapEditMode editMode) const;
    void updateSliderVisibility();
    void repositionSliders();
    juce::Rectangle<int> getLabelBounds() const;

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

bool TapEditItem::isTapSelected() const
{
    Impl &impl = *impl_;
    return impl.tapSelected_;
}

void TapEditItem::setTapSelected(bool selected)
{
    Impl &impl = *impl_;

    if (impl.tapSelected_ == selected)
        return;

    impl.tapSelected_ = selected;
    repaint();
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
    juce::Colour tapLabelBackgroundColour = findColour(impl.tapSelected_ ? TapEditScreen::tapLabelSelectedBackgroundColourId : TapEditScreen::tapLabelBackgroundColourId);
    juce::Colour tapLabelTextColour = findColour(TapEditScreen::tapLabelTextColourId);

    char labelTextCstr[2];
    labelTextCstr[0] = (char)(impl.itemNumber_ + 'A');
    labelTextCstr[1] = '\0';

    juce::Rectangle<int> labelBounds = impl.getLabelBounds();
    g.setColour(tapLabelBackgroundColour);
    g.fillRoundedRectangle(labelBounds.toFloat(), 3.0f);
    g.setColour(tapLabelTextColour);
    g.drawText(labelTextCstr, labelBounds, juce::Justification::centred);
}

void TapEditItem::mouseDown(const juce::MouseEvent &e)
{
    Impl &impl = *impl_;
    juce::Rectangle<int> bounds = getLocalBounds();

    if (impl.dragChangeId_ == GDP_NONE && e.y >= bounds.getBottom() - getLabelHeight()) {
        impl.screen_->setOnlyTapSelected(impl.itemNumber_);
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
                float newDelay = screen->alignDelayToGrid(screen->getDelayForX(bounds.toFloat().getCentreX()));
                float halfWidth = 0.5f * (float)bounds.getWidth();
                bounds.setX(juce::roundToInt(screen->getXForDelay(newDelay) - halfWidth));
                bounds.setY(previousBounds.getY());
            }
        private:
            TapEditScreen *screen_ = nullptr;
        };
        TapEditScreen *screen = impl.screen_;
        TapConstrainer constrainer(screen);
        impl.dragger_.dragComponent(this, e, &constrainer);
        float newDelay = screen->alignDelayToGrid(screen->getDelayForX(getBounds().toFloat().getCentreX()));
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
    juce::Rectangle<int> sliderBounds = bounds.withTrimmedBottom(getLabelHeight());
    sliderBounds = sliderBounds.withSizeKeepingCentre(8, sliderBounds.getHeight());

    for (const auto &sliderPair : sliders_) {
        TapSlider *slider = sliderPair.second.get();
        slider->setBounds(sliderBounds);
    }
}

juce::Rectangle<int> TapEditItem::Impl::getLabelBounds() const
{
    TapEditItem *self = self_;
    return self->getLocalBounds().removeFromBottom(getLabelHeight());
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
