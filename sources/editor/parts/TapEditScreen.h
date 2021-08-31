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
        tapSliderBackgroundColourId,
        tapSliderFillColourId,
        minorIntervalTickColourId,
        majorIntervalTickColourId,
        superMajorIntervalTickColourId,
        lassoFillColourId,
        lassoOutlineColourId,
        textColourId,
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
    void autoZoomTimeRange();

    juce::Rectangle<int> getLocalBoundsNoMargin() const;
    juce::Rectangle<int> getScreenArea() const;
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
    void resized() override;

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
struct TapMiniMapValue {
    float delay;
    juce::Range<float> range;
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
    bool getReferenceLineY(TapEditMode mode, float &lineY, juce::Component *relativeTo = nullptr) const;

    TapEditMode getEditMode() const noexcept;
    void setEditMode(TapEditMode mode);

    float getTapValue(GdParameter id) const;
    void setTapValue(GdParameter id, float value, juce::NotificationType nt = juce::sendNotificationSync);

    TapMiniMapValue getMinimapValues() const;

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
    void displayValues(const TapMiniMapValue values[], int count);

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
