// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "GdDefs.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

//------------------------------------------------------------------------------
enum TapEditMode {
    kTapEditOff,
    kTapEditCutoff,
    kTapEditResonance,
    kTapEditTune,
    kTapEditPan,
    kTapEditLevel,
};

//------------------------------------------------------------------------------
class TapEditScreen final : public juce::Component {
public:
    enum ColourIds {
        lineColourId = 0x1000,
        screenContourColourId,
        intervalFillColourId,
        intervalContourColourId,
        tapLineColourId,
        tapLabelBackgroundColourId,
        tapLabelSelectedBackgroundColourId,
        tapLabelTextColourId,
        minorIntervalTickColourId,
        majorIntervalTickColourId,
        superMajorIntervalTickColourId,
        lassoFillColourId,
        lassoOutlineColourId,
    };

    TapEditScreen();
    ~TapEditScreen() override;

    TapEditMode getEditMode() const noexcept;
    void setEditMode(TapEditMode mode);

    juce::Range<float> getTimeRange() const noexcept;
    void setTimeRange(juce::Range<float> newTimeRange);

    float getTapValue(GdParameter id) const;
    void setTapValue(GdParameter id, float value, juce::NotificationType nt = juce::sendNotificationSync);

    bool isTapSelected(int tapNumber) const;
    void setAllTapsSelected(bool selected);
    void setOnlyTapSelected(int selectedTapNumber);

    double getBPM() const;
    void setBPM(double bpm);

    void beginTap();
    void endTap();

    void updateItemSizeAndPosition(int tapNumber);
    void updateAllItemSizesAndPositions();
    float getXForDelay(float delay) const;
    float getDelayForX(float x) const;
    float alignDelayToGrid(float delay) const;

    juce::Rectangle<int> getLocalBoundsNoMargin() const;
    juce::Rectangle<int> getScreenBounds() const;
    juce::Rectangle<int> getIntervalsRow() const;
    juce::Rectangle<int> getSlidersRow() const;

    class Listener {
    public:
        virtual ~Listener() {}

        virtual void tappingHasStarted(TapEditScreen *) {}
        virtual void tappingHasEnded(TapEditScreen *) {}

        virtual void tapEditStarted(TapEditScreen *, GdParameter id) { (void)id; }
        virtual void tapEditEnded(TapEditScreen *, GdParameter id) { (void)id; }
        virtual void tapValueChanged(TapEditScreen *, GdParameter id, float value) { (void)id; (void)value; }
    };

    void addListener(Listener *listener);
    void removeListener(Listener *listener);

protected:
    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    bool keyPressed(const juce::KeyPress &e) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

//------------------------------------------------------------------------------
struct TapEditData {
    bool enabled = false;
    float delay = 0;
};

//------------------------------------------------------------------------------
class TapEditItem final : public juce::Component {
public:
    explicit TapEditItem(TapEditScreen *screen, int itemNumber);
    ~TapEditItem() override;
    int getItemNumber() const noexcept;
    const TapEditData &getData() const noexcept;
    static constexpr int getLabelWidth() noexcept { return 20; }
    static constexpr int getLabelHeight() noexcept { return 20; }

    TapEditMode getEditMode() const noexcept;
    void setEditMode(TapEditMode mode);

    float getTapValue(GdParameter id) const;
    void setTapValue(GdParameter id, float value, juce::NotificationType nt = juce::sendNotificationSync);

    bool isTapSelected() const;
    void setTapSelected(bool selected);

    class Listener {
    public:
        virtual ~Listener() {}

        virtual void tapEditStarted(TapEditItem *, GdParameter id) { (void)id; }
        virtual void tapEditEnded(TapEditItem *, GdParameter id) { (void)id; }
        virtual void tapValueChanged(TapEditItem *, GdParameter id, float value) { (void)id; (void)value; }
    };

    void addListener(Listener *listener);
    void removeListener(Listener *listener);

protected:
    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void moved() override;
    void resized() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

//------------------------------------------------------------------------------
class TapMiniMap final : public juce::Component {
public:
    TapMiniMap();
    ~TapMiniMap() override;
    void setTimeRange(juce::Range<float> timeRange, juce::NotificationType nt = juce::sendNotificationSync);

    class Listener {
    public:
        virtual ~Listener() {}
        virtual void miniMapRangeChanged(TapMiniMap *, juce::Range<float> range) { (void)range; }
    };

    void addListener(Listener *listener);
    void removeListener(Listener *listener);

protected:
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void paint(juce::Graphics &g) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
