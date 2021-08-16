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
        tapLineColourId,
    };

    TapEditScreen();
    ~TapEditScreen() override;

    TapEditMode getEditMode() const noexcept;
    void setEditMode(TapEditMode mode);

    float getTimeRange() const noexcept;
    void setTimeRange(float maxTime);

    void setTapValue(GdParameter id, float value, juce::NotificationType nt = juce::sendNotificationSync);

    void beginTap();
    void endTap();

    void updateItemSizeAndPosition(int tapNumber);
    float getXForDelay(float delay) const;
    float getDelayForX(float x) const;

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

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

//------------------------------------------------------------------------------
struct TapEditData {
    bool enabled = false;
    float delay = 0;
    float level = 0;
};

//------------------------------------------------------------------------------
class TapEditItem final : public juce::Component {
public:
    explicit TapEditItem(TapEditScreen *screen, int itemNumber);
    ~TapEditItem() override;
    int getItemNumber() const noexcept;
    const TapEditData &getData() const noexcept;
    int getLabelWidth() const noexcept;
    int getLabelHeight() const noexcept;

    TapEditMode getEditMode() const noexcept;
    void setEditMode(TapEditMode mode);

    void setTapEnabled(bool enabled, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapDelay(float delay, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapLPFCutoff(float cutoff, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapHPFCutoff(float cutoff, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapResonance(float resonance, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapTune(float tune, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapPan(float pan, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapLevel(float level, juce::NotificationType nt = juce::sendNotificationSync);

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
