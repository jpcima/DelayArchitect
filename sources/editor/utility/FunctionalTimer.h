// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <utility>

class FunctionalTimer : public juce::Timer {
public:
    virtual ~FunctionalTimer() {}
    template <class T> static juce::Timer *create(T fn);
};

template <class T>
class FunctionalTimerT final : public FunctionalTimer {
public:
    explicit FunctionalTimerT(T fn) : fn_(std::move(fn)) {}
    void timerCallback() override { fn_(); }
private:
    T fn_;
};

template <class T>
juce::Timer *FunctionalTimer::create(T fn)
{
    return new FunctionalTimerT<T>(std::move(fn));
}
