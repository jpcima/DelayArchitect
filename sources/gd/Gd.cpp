// SPDX-License-Identifier: AGPL-3.0-or-later
#include "Gd.h"
#include "GdNetwork.h"
#include "utility/LinearSmoother.h"
#include "utility/NextPowerOfTwo.h"
#include "utility/Volume.h"
#include "utility/StdcLocale.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cassert>

struct Gd {
    unsigned numinputs_ = 0;
    std::unique_ptr<GdNetwork> network_;
    float samplerate_ = 0;
    unsigned bufsize_ = 0;

    LinearSmoother smoothMixDryLinear_;
    LinearSmoother smoothMixWetLinear_;

    std::vector<float> temp_[4];

    float parameters_[GD_PARAMETER_COUNT] {};
};

Gd *GdNew(unsigned numinputs, unsigned numoutputs)
{
    if (numoutputs != 2)
        return nullptr;

    Gd *gd = new Gd;
    gd->numinputs_ = numinputs;

    if (numinputs == 1)
        gd->network_.reset(new GdNetwork(GdNetwork::Mono));
    else if (numinputs == 2)
        gd->network_.reset(new GdNetwork(GdNetwork::Stereo));
    else
        return nullptr;

    gd->smoothMixDryLinear_.setTimeConstant(GdParamSmoothTime);
    gd->smoothMixWetLinear_.setTimeConstant(GdParamSmoothTime);

    const float defaultSampleRate = 44100;
    const unsigned defaultBlockSize = 128;

    GdSetSampleRate(gd, defaultSampleRate);
    GdSetBufferSize(gd, defaultBlockSize);

    gd->smoothMixDryLinear_.setSampleRate(defaultSampleRate);
    gd->smoothMixWetLinear_.setSampleRate(defaultSampleRate);

    float *parameters = gd->parameters_;
    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i)
        parameters[i] = GdParameterDefault((GdParameter)i);

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        bool force = true;
        GdSetParameterEx(gd, (GdParameter)i, parameters[i], force);
    }

    return gd;
}

void GdFree(Gd *gd)
{
    delete gd;
}

void GdClear(Gd *gd)
{
    gd->smoothMixDryLinear_.clearToTarget();
    gd->smoothMixWetLinear_.clearToTarget();

    gd->network_->clear();
}

void GdSetSampleRate(Gd *gd, float samplerate)
{
    if (gd->samplerate_ == samplerate)
        return;

    gd->samplerate_ = samplerate;

    gd->network_->setSampleRate(samplerate);
}

void GdSetBufferSize(Gd *gd, unsigned bufsize)
{
    if (gd->bufsize_ == bufsize)
        return;

    gd->bufsize_ = bufsize;

    for (std::vector<float> &temp : gd->temp_)
        temp.resize(bufsize);

    gd->network_->setBufferSize(bufsize);
}

void GdProcess(Gd *gd, const float *inputs[], float *outputs[], unsigned count)
{
    if (count > gd->bufsize_) { // safety measure
        GdSetBufferSize(gd, nextPowerOfTwo(count));
    }

    ///
    float *dry = gd->temp_[0].data();
    float *wet = gd->temp_[1].data();

    gd->smoothMixDryLinear_.nextBlock(dry, count);
    gd->smoothMixWetLinear_.nextBlock(wet, count);

    ///
    unsigned numinputs = gd->numinputs_;
    float *intermediates[] = { gd->temp_[2].data(), gd->temp_[3].data() };

    for (unsigned i = 0; i < numinputs; ++i)
        std::copy_n(inputs[i], count, intermediates[i]);

    gd->network_->process(intermediates, dry, wet, outputs, count);
}

void GdSetTempo(Gd *gd, float tempo)
{
    gd->network_->setTempo(tempo);
}

void GdSetParameter(Gd *gd, GdParameter p, float value)
{
    bool force = false;
    GdSetParameterEx(gd, p, value, force);
}

void GdSetParameterEx(Gd *gd, GdParameter p, float value, bool force)
{
    float *parameters = gd->parameters_;

    value = GdAdjustParameter(p, value);
    if (!force && parameters[p] == value)
        return;

    parameters[p] = value;

    switch ((int)p) {
    case GDP_MIX_DRY:
        gd->smoothMixDryLinear_.setTarget((value <= GdMinMixGainDB) ? 0.0f :
            db2linear(value));
        break;
    case GDP_MIX_WET:
        gd->smoothMixWetLinear_.setTarget((value <= GdMinMixGainDB) ? 0.0f :
            db2linear(value));
        break;
    }

    gd->network_->setParameter((unsigned)p, value);
}

float GdGetParameter(Gd *gd, GdParameter p)
{
    if (p >= GD_PARAMETER_COUNT)
        return 0;
    return gd->parameters_[p];
}

float GdAdjustParameter(GdParameter p, float value)
{
    unsigned flags = GdParameterFlags(p);

    if (flags & GDP_BOOLEAN)
        value = value ? 1.0f : 0.0f;
    else {
        float min = GdParameterMin(p);
        float max = GdParameterMax(p);
        value = (value < min) ? min : value;
        value = (value > max) ? max : value;
        if (flags & GDP_INTEGER)
            value = std::round(value);
    }

    return value;
}

unsigned GdParameterCount()
{
    return GD_PARAMETER_COUNT;
}

const char *GdParameterName(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return #p;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return nullptr;
    }
}

GdParameter GdParameterByName(const char *name)
{
    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        if (!std::strcmp(name, GdParameterName((GdParameter)i)))
            return (GdParameter)i;
    }
    return (GdParameter)-1;
}

GdRange GdParameterRange(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return (GdMakeRange range);
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return GdDefaultRange;
    }
}

float GdParameterMin(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return (GdMakeRange range).start;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return 0;
    }
}

float GdParameterMax(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return (GdMakeRange range).end;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return 0;
    }
}

float GdParameterDefault(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return def;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return 0;
    }
}

unsigned GdParameterFlags(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return flags;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return 0;
    }
}

const char *GdParameterLabel(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return label;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return nullptr;
    }
}

const char *GdParameterUnit(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return unit;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return nullptr;
    }
}

#if 0
static char const* const GdTapNames[GdMaxLines + 1] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    nullptr
};
#endif
static char const* const GdTapLabels[GdMaxLines + 1] = {
    "Tap A", "Tap B", "Tap C", "Tap D", "Tap E", "Tap F", "Tap G", "Tap H", "Tap I", "Tap J", "Tap K", "Tap L", "Tap M",
    "Tap N", "Tap O", "Tap P", "Tap Q", "Tap R", "Tap S", "Tap T", "Tap U", "Tap V", "Tap W", "Tap X", "Tap Y", "Tap Z",
    nullptr
};
static char const* const GdFilterLabels[GdNumFilterTypes + 1] = {
    "6 dB/oct",
    "12 dB/oct",
    nullptr
};

const char *const *GdParameterChoices(GdParameter p)
{
    if ((int)p >= GDP_TAP_A_ENABLE)
        p = (GdParameter)(((int)p - GDP_TAP_A_ENABLE) % (GDP_TAP_B_ENABLE - GDP_TAP_A_ENABLE) + GDP_TAP_A_ENABLE);

    switch (p) {
    case GDP_FEEDBACK_TAP:
        return GdTapLabels;
    case GDP_TAP_A_FILTER:
        return GdFilterLabels;
    default:
        return nullptr;
    }
}

int GdParameterGroup(GdParameter p)
{
    switch (p) {
    #define EACH(p, range, def, flags, label, unit, group) case GDP_##p: return group;
    GD_EACH_PARAMETER(EACH)
    #undef EACH
    default:
        return 0;
    }
}

GD_API const char *GdGroupName(GdParameter p)
{
    int group = GdParameterGroup(p);

    if (group >= 0 && group < GdMaxLines)
        return GdTapLabels[group];

    return nullptr;
}

GD_API const char *GdGroupLabel(GdParameter p)
{
    int group = GdParameterGroup(p);

    if (group >= 0 && group < GdMaxLines)
        return GdTapLabels[group];

    return nullptr;
}

void GdFormatParameterValue(GdParameter p, float value, char *text, unsigned textsize)
{
    int flags = GdParameterFlags(p);
    const char *unit = GdParameterUnit(p);

    if (!unit)
        return;

    if (flags & GDP_CHOICE) {
        unsigned index = 0;
        const char *choice = nullptr;
        const char *const *choices = GdParameterChoices(p);
        for (index = 0; !choice && choices[index]; ++index) {
            if (index == (unsigned)value)
                choice = choices[index];
        }
        snprintf(text, textsize, "%s", choice ? choice : "");
    }
    else if (flags & GDP_BOOLEAN)
        snprintf(text, textsize, "%s", value ? "On" : "Off");
    else if (flags & GDP_INTEGER)
        snprintf(text, textsize, "%d%s%s", (int)value, unit ? " " : "", unit);
    else {
        Locale loc = Locale::create(LC_NUMERIC, "C");
        loc.snprintf(text, textsize, "%.2f%s%s", value, unit ? " " : "", unit);
    }

    if (textsize > 0)
        text[textsize - 1] = '\0';
}
