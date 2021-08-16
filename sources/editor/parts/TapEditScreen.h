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

    void setTapEnabled(int tapNumber, bool enabled, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapDelay(int tapNumber, float delay, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapLPFCutoff(int tapNumber, float cutoff, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapHPFCutoff(int tapNumber, float cutoff, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapResonance(int tapNumber, float resonance, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapTune(int tapNumber, float tune, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapPan(int tapNumber, float pan, juce::NotificationType nt = juce::sendNotificationSync);
    void setTapLevel(int tapNumber, float level, juce::NotificationType nt = juce::sendNotificationSync);

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

        enum ChangeId {
            kChangeEnabled = GDP_TAP_A_ENABLE,
            kChangeDelay = GDP_TAP_A_DELAY,
            kChangeLPFCutoff = GDP_TAP_A_LPF_CUTOFF,
            kChangeHPFCutoff = GDP_TAP_A_HPF_CUTOFF,
            kChangeResonance = GDP_TAP_A_RESONANCE,
            kChangeTune = GDP_TAP_A_TUNE,
            kChangePan = GDP_TAP_A_PAN,
            kChangeLevel = GDP_TAP_A_LEVEL,
        };

        virtual void tapEditStarted(TapEditScreen *, int tapNumber, ChangeId id) { (void)tapNumber; (void)id; }
        virtual void tapEditEnded(TapEditScreen *, int tapNumber, ChangeId id) { (void)tapNumber; (void)id; }
        virtual void tapValueChanged(TapEditScreen *, int tapNumber, ChangeId id, float value) { (void)tapNumber; (void)id; (void)value; }
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

        using ChangeId = TapEditScreen::Listener::ChangeId;

        virtual void tapEditStarted(TapEditItem *, ChangeId id) { (void)id; }
        virtual void tapEditEnded(TapEditItem *, ChangeId id) { (void)id; }
        virtual void tapValueChanged(TapEditItem *, ChangeId id, float value) { (void)id; (void)value; }
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
