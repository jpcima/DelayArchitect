#include "GdShifterSuperCollider.h"
#include "utility/Clamp.h"
#include "utility/NextPowerOfTwo.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

/**/
// hash function for integers
static inline int32_t Hash(int32_t inKey) {
    // Thomas Wang's integer hash.
    // http://www.concentric.net/~Ttwang/tech/inthash.htm
    // a faster hash for integers. also very good.
    uint32_t hash = (uint32_t)inKey;
    hash += ~(hash << 15);
    hash ^= hash >> 10;
    hash += hash << 3;
    hash ^= hash >> 6;
    hash += ~(hash << 11);
    hash ^= hash >> 16;
    return (int32_t)hash;
}

static inline void initrand(uint32_t seed, uint32_t &s1, uint32_t &s2, uint32_t &s3)
{
    // humans tend to use small seeds - mess up the bits
    seed = (uint32_t)Hash((int)seed);

    // initialize seeds using the given seed value taking care of
    // the requirements. The constants below are arbitrary otherwise
    s1 = 1243598713U ^ seed;
    if (s1 < 2)
        s1 = 1243598713U;
    s2 = 3093459404U ^ seed;
    if (s2 < 8)
        s2 = 3093459404U;
    s3 = 1821928721U ^ seed;
    if (s3 < 16)
        s3 = 1821928721U;
}

/// Generate a random 32 bit number
static inline uint32_t trand(uint32_t &s1, uint32_t &s2, uint32_t &s3) {
    s1 = ((s1 & (uint32_t)-2) << 12) ^ (((s1 << 13) ^ s1) >> 19);
    s2 = ((s2 & (uint32_t)-8) << 4) ^ (((s2 << 2) ^ s2) >> 25);
    s3 = ((s3 & (uint32_t)-16) << 17) ^ (((s3 << 3) ^ s3) >> 11);
    return s1 ^ s2 ^ s3;
}

/// Generate a float from 0.0 to 0.999...
static inline float frand(uint32_t &s1, uint32_t &s2, uint32_t &s3) {
    union {
        uint32_t i;
        float f;
    } u;
    u.i = 0x3F800000 | (trand(s1, s2, s3) >> 9);
    return u.f - 1.f;
}

/// Generate a float from -1.0 to +0.999...
static inline float frand2(uint32_t &s1, uint32_t &s2, uint32_t &s3) {
    union {
        uint32_t i;
        float f;
    } u;
    u.i = 0x40000000 | (trand(s1, s2, s3) >> 9);
    return u.f - 3.f;
}
/**/

GdShifter::GdShifter()
{
    setSampleRate(44100);
}

GdShifter::~GdShifter()
{
}

void GdShifter::clear()
{
    PitchShift *unit = &unit_;
    std::fill(delayBuffer_.begin(), delayBuffer_.end(), 0.0f);
    initrand(0, rgen_.s1, rgen_.s2, rgen_.s3);
    /**/

    float slope;
    long framesize = unit->framesize;

    calc_ = &GdShifter::processNextZ/*SETCALC(PitchShift_next_z)*/;
    /**/if (shift_ == 1.0f) calc_ = &GdShifter::copyNext;

    /**dlybuf = ZIN0(0);
    ZOUT0(0) = 0.f*/;

    unit->iwrphase = 0;
    unit->numoutput = 0;

    unit->slope = slope = 2.f / framesize;
    unit->stage = 3;
    unit->counter = framesize >> 2;
    unit->ramp1 = 0.5;
    unit->ramp2 = 1.0;
    unit->ramp3 = 0.5;
    unit->ramp4 = 0.0;

    unit->ramp1_slope = -slope;
    unit->ramp2_slope = -slope;
    unit->ramp3_slope = slope;
    unit->ramp4_slope = slope;

    /*dlybuf[last] = 0.f; // put a few zeroes where we start the read heads
    dlybuf[last - 1] = 0.f;
    dlybuf[last - 2] = 0.f;*/

    // start all read heads 2 samples behind the write head
    unit->dsamp1 = unit->dsamp2 = unit->dsamp3 = unit->dsamp4 = 2.f;
    // pch ratio is initially zero for the read heads
    unit->dsamp1_slope = unit->dsamp2_slope = unit->dsamp3_slope = unit->dsamp4_slope = 1.f;
}

void GdShifter::setSampleRate(float sampleRate)
{
    if (sampleRate_ == sampleRate)
        return;

    sampleRate_ = sampleRate;
    postUpdateSampleRateOrBufferSize();
}

void GdShifter::setBufferSize(unsigned bufferSize)
{
    if (bufferSize_ == bufferSize)
        return;

    bufferSize_ = bufferSize;
    postUpdateSampleRateOrBufferSize();
}

void GdShifter::setShift(float shiftLinear)
{
    float oldShift = shift_;

    if (oldShift == shiftLinear)
        return;

    shift_ = shiftLinear;
    if (oldShift == 1.0f)
        clear();
    else if (shiftLinear == 1.0f)
        calc_ = &GdShifter::copyNext;
}

void GdShifter::postUpdateSampleRateOrBufferSize()
{
    PitchShift *unit = &unit_;
    float SAMPLERATE = sampleRate_;
    float SAMPLEDUR = 1 / SAMPLERATE;
    long BUFLENGTH = bufferSize_;
    /**/

    long delaybufsize;
    float *dlybuf;
    float winsize;
    float fdelaylen;
    long last;

    winsize = getWindowSize()/*ZIN0(1)*/;

    // TODO: why does scsynth freeze if the window size is <= 2 samples?

    // Nobody needs windows that small for pitch shifting anyway, so we will
    // simply clamp the window size to 3.
    float minimum_winsize = 3.f * SAMPLEDUR;
    if (winsize < minimum_winsize) {
        winsize = minimum_winsize;
    }

    delaybufsize = (long)std::ceil(winsize * SAMPLERATE * 3.f + 3.f);
    fdelaylen = delaybufsize - 3;

    delaybufsize = delaybufsize + BUFLENGTH;
    delaybufsize = nextPowerOfTwo/*NEXTPOWEROFTWO*/(delaybufsize); // round up to next power of two
    dlybuf = (delayBuffer_.clear(), delayBuffer_.resize(delaybufsize), delayBuffer_.data())/*(float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float))*/;

    unit->dlybuf = dlybuf;
    unit->idelaylen = delaybufsize;
    unit->fdelaylen = fdelaylen;
    unit->mask = last = (delaybufsize - 1);

    unit->framesize = ((long)(winsize * SAMPLERATE) + 2) & ~3;

    /**/
    clear();
}

float GdShifter::processOne(float input)
{
    float output;
    process(&input, &output, 1);
    return output;
}

void GdShifter::processNext(const float *input, float *output, unsigned count)
{
    PitchShift *unit = &unit_;
    float SAMPLERATE = sampleRate_;
    /**/
    float *out, *in, *dlybuf;
    float disppchratio, pchratio, pchratio1, value;
    float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
    float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
    float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
    float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
    float fdelaylen, d1, d2, frac, slope, samp_slope, startpos, winsize, pchdisp, timedisp;
    long remain, nsmps, idelaylen, irdphase, irdphaseb, iwrphase, mask, idsamp;
    long counter, stage, framesize;

    uint32_t s1 = rgen_.s1, s2 = rgen_.s2, s3 = rgen_.s3; /*RGET*/

    out = /*ZOUT(0)*/output;
    in = /*ZIN(0)*/(float *)input;

    /*pchratio = ZIN0(2);*/
    winsize = getWindowSize()/*ZIN0(1)*/;
    pchdisp = getPitchDispersion()/*ZIN0(3)*/;
    timedisp = getTimeDispersion()/*ZIN0(4)*/;
    timedisp = clamp/*sc_clip*/(timedisp, 0.f, winsize) * SAMPLERATE;

    dlybuf = unit->dlybuf;
    fdelaylen = unit->fdelaylen;
    idelaylen = unit->idelaylen;
    iwrphase = unit->iwrphase;

    counter = unit->counter;
    stage = unit->stage;
    mask = unit->mask;
    framesize = unit->framesize;

    dsamp1 = unit->dsamp1;
    dsamp2 = unit->dsamp2;
    dsamp3 = unit->dsamp3;
    dsamp4 = unit->dsamp4;

    dsamp1_slope = unit->dsamp1_slope;
    dsamp2_slope = unit->dsamp2_slope;
    dsamp3_slope = unit->dsamp3_slope;
    dsamp4_slope = unit->dsamp4_slope;

    ramp1 = unit->ramp1;
    ramp2 = unit->ramp2;
    ramp3 = unit->ramp3;
    ramp4 = unit->ramp4;

    ramp1_slope = unit->ramp1_slope;
    ramp2_slope = unit->ramp2_slope;
    ramp3_slope = unit->ramp3_slope;
    ramp4_slope = unit->ramp4_slope;

    slope = unit->slope;

    remain = count/*inNumSamples*/;
    while (remain) {
        if (counter <= 0) {
            counter = framesize >> 2;
            unit->stage = stage = (stage + 1) & 3;
            disppchratio = /*pchratio*/shift_;
            if (pchdisp != 0.f) {
                disppchratio += (pchdisp * frand2(s1, s2, s3));
            }
            disppchratio = clamp/*sc_clip*/(disppchratio, 0.f, 4.f);
            pchratio1 = disppchratio - 1.f;
            samp_slope = -pchratio1;
            startpos = pchratio1 < 0.f ? 2.f : framesize * pchratio1 + 2.f;
            startpos += (timedisp * frand(s1, s2, s3));
            switch (stage) {
            case 0:
                unit->dsamp1_slope = dsamp1_slope = samp_slope;
                dsamp1 = startpos;
                ramp1 = 0.0;
                unit->ramp1_slope = ramp1_slope = slope;
                unit->ramp3_slope = ramp3_slope = -slope;
                break;
            case 1:
                unit->dsamp2_slope = dsamp2_slope = samp_slope;
                dsamp2 = startpos;
                ramp2 = 0.0;
                unit->ramp2_slope = ramp2_slope = slope;
                unit->ramp4_slope = ramp4_slope = -slope;
                break;
            case 2:
                unit->dsamp3_slope = dsamp3_slope = samp_slope;
                dsamp3 = startpos;
                ramp3 = 0.0;
                unit->ramp3_slope = ramp3_slope = slope;
                unit->ramp1_slope = ramp1_slope = -slope;
                break;
            case 3:
                unit->dsamp4_slope = dsamp4_slope = samp_slope;
                dsamp4 = startpos;
                ramp4 = 0.0;
                unit->ramp2_slope = ramp2_slope = -slope;
                unit->ramp4_slope = ramp4_slope = slope;
                break;
            }
            /*Print("%d %d    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
                counter, stage, dsamp1_slope, dsamp2_slope, dsamp3_slope, dsamp4_slope,
                    dsamp1, dsamp2, dsamp3, dsamp4,
                    ramp1, ramp2, ramp3, ramp4);*/
        }

        nsmps = std::min/*sc_min*/(remain, counter);
        remain -= nsmps;
        counter -= nsmps;

        for (long xxi = 0; xxi < nsmps; ++xxi) {/*LOOP(nsmps,*/
            iwrphase = (iwrphase + 1) & mask;

            dsamp1 += dsamp1_slope; idsamp = (long)dsamp1; frac = dsamp1 - idsamp;
            irdphase = (iwrphase - idsamp) & mask; irdphaseb = (irdphase - 1) & mask; d1 = dlybuf[irdphase];
            d2 = dlybuf[irdphaseb]; value = (d1 + frac * (d2 - d1)) * ramp1; ramp1 += ramp1_slope;

            dsamp2 += dsamp2_slope; idsamp = (long)dsamp2; frac = dsamp2 - idsamp;
            irdphase = (iwrphase - idsamp) & mask; irdphaseb = (irdphase - 1) & mask; d1 = dlybuf[irdphase];
            d2 = dlybuf[irdphaseb]; value += (d1 + frac * (d2 - d1)) * ramp2; ramp2 += ramp2_slope;

            dsamp3 += dsamp3_slope; idsamp = (long)dsamp3; frac = dsamp3 - idsamp;
            irdphase = (iwrphase - idsamp) & mask; irdphaseb = (irdphase - 1) & mask; d1 = dlybuf[irdphase];
            d2 = dlybuf[irdphaseb]; value += (d1 + frac * (d2 - d1)) * ramp3; ramp3 += ramp3_slope;

            dsamp4 += dsamp4_slope; idsamp = (long)dsamp4; frac = dsamp4 - idsamp;
            irdphase = (iwrphase - idsamp) & mask; irdphaseb = (irdphase - 1) & mask; d1 = dlybuf[irdphase];
            d2 = dlybuf[irdphaseb]; value += (d1 + frac * (d2 - d1)) * ramp4; ramp4 += ramp4_slope;

            dlybuf[iwrphase] = *in++/*ZXP(in)*/; *out++/*ZXP(out)*/ = value *= 0.5;
        }/*);*/
    }

    unit->counter = counter;

    unit->dsamp1 = dsamp1;
    unit->dsamp2 = dsamp2;
    unit->dsamp3 = dsamp3;
    unit->dsamp4 = dsamp4;

    unit->ramp1 = ramp1;
    unit->ramp2 = ramp2;
    unit->ramp3 = ramp3;
    unit->ramp4 = ramp4;

    unit->iwrphase = iwrphase;

    rgen_.s1 = s1; rgen_.s2 = s2; rgen_.s3 = s3;/*RPUT*/
}

void GdShifter::processNextZ(const float *input, float *output, unsigned count)
{
    PitchShift *unit = &unit_;
    float SAMPLERATE = sampleRate_;
    /**/

    float *out, *in, *dlybuf;
    float disppchratio, pchratio, pchratio1, value;
    float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
    float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
    float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
    float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
    float fdelaylen, d1, d2, frac, slope, samp_slope, startpos, winsize, pchdisp, timedisp;
    long remain, nsmps, idelaylen, irdphase, irdphaseb, iwrphase;
    long mask, idsamp;
    long counter, stage, framesize, numoutput;

    uint32_t s1 = rgen_.s1, s2 = rgen_.s2, s3 = rgen_.s3; /*RGET*/

    out = /*ZOUT(0)*/output;
    in = /*ZIN(0)*/(float *)input;
    /*pchratio = ZIN0(2);*/
    winsize = getWindowSize()/*ZIN0(1)*/;
    pchdisp = getPitchDispersion()/*ZIN0(3)*/;
    timedisp = getTimeDispersion()/*ZIN0(4)*/;
    timedisp = clamp/*sc_clip*/(timedisp, 0.f, winsize) * SAMPLERATE;

    dlybuf = unit->dlybuf;
    fdelaylen = unit->fdelaylen;
    idelaylen = unit->idelaylen;
    iwrphase = unit->iwrphase;
    numoutput = unit->numoutput;

    counter = unit->counter;
    stage = unit->stage;
    mask = unit->mask;
    framesize = unit->framesize;

    dsamp1 = unit->dsamp1;
    dsamp2 = unit->dsamp2;
    dsamp3 = unit->dsamp3;
    dsamp4 = unit->dsamp4;

    dsamp1_slope = unit->dsamp1_slope;
    dsamp2_slope = unit->dsamp2_slope;
    dsamp3_slope = unit->dsamp3_slope;
    dsamp4_slope = unit->dsamp4_slope;

    ramp1 = unit->ramp1;
    ramp2 = unit->ramp2;
    ramp3 = unit->ramp3;
    ramp4 = unit->ramp4;

    ramp1_slope = unit->ramp1_slope;
    ramp2_slope = unit->ramp2_slope;
    ramp3_slope = unit->ramp3_slope;
    ramp4_slope = unit->ramp4_slope;

    slope = unit->slope;

    remain = count/*inNumSamples*/;
    while (remain) {
        if (counter <= 0) {
            counter = framesize >> 2;
            unit->stage = stage = (stage + 1) & 3;
            disppchratio = /*pchratio*/shift_;
            if (pchdisp != 0.f) {
                disppchratio += (pchdisp * frand2(s1, s2, s3));
            }
            disppchratio = clamp/*sc_clip*/(disppchratio, 0.f, 4.f);
            pchratio1 = disppchratio - 1.f;
            samp_slope = -pchratio1;
            startpos = pchratio1 < 0.f ? 2.f : framesize * pchratio1 + 2.f;
            startpos += (timedisp * frand(s1, s2, s3));
            switch (stage) {
            case 0:
                unit->dsamp1_slope = dsamp1_slope = samp_slope;
                dsamp1 = startpos;
                ramp1 = 0.0;
                unit->ramp1_slope = ramp1_slope = slope;
                unit->ramp3_slope = ramp3_slope = -slope;
                break;
            case 1:
                unit->dsamp2_slope = dsamp2_slope = samp_slope;
                dsamp2 = startpos;
                ramp2 = 0.0;
                unit->ramp2_slope = ramp2_slope = slope;
                unit->ramp4_slope = ramp4_slope = -slope;
                break;
            case 2:
                unit->dsamp3_slope = dsamp3_slope = samp_slope;
                dsamp3 = startpos;
                ramp3 = 0.0;
                unit->ramp3_slope = ramp3_slope = slope;
                unit->ramp1_slope = ramp1_slope = -slope;
                break;
            case 3:
                unit->dsamp4_slope = dsamp4_slope = samp_slope;
                dsamp4 = startpos;
                ramp4 = 0.0;
                unit->ramp2_slope = ramp2_slope = -slope;
                unit->ramp4_slope = ramp4_slope = slope;
                break;
            }
            /*Print("z %d %d    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
                counter, stage, dsamp1_slope, dsamp2_slope, dsamp3_slope, dsamp4_slope,
                    dsamp1, dsamp2, dsamp3, dsamp4,
                    ramp1, ramp2, ramp3, ramp4);*/
        }
        nsmps = std::min/*sc_min*/(remain, counter);
        remain -= nsmps;
        counter -= nsmps;

        while (nsmps--) {
            numoutput++;
            iwrphase = (iwrphase + 1) & mask;

            dsamp1 += dsamp1_slope;
            idsamp = (long)dsamp1;
            frac = dsamp1 - idsamp;
            irdphase = (iwrphase - idsamp) & mask;
            irdphaseb = (irdphase - 1) & mask;
            if (numoutput < idelaylen) {
                if (irdphase > iwrphase) {
                    value = 0.f;
                } else if (irdphaseb > iwrphase) {
                    d1 = dlybuf[irdphase];
                    value = (d1 - frac * d1) * ramp1;
                } else {
                    d1 = dlybuf[irdphase];
                    d2 = dlybuf[irdphaseb];
                    value = (d1 + frac * (d2 - d1)) * ramp1;
                }
            } else {
                d1 = dlybuf[irdphase];
                d2 = dlybuf[irdphaseb];
                value = (d1 + frac * (d2 - d1)) * ramp1;
            }
            ramp1 += ramp1_slope;

            dsamp2 += dsamp2_slope;
            idsamp = (long)dsamp2;
            frac = dsamp2 - idsamp;
            irdphase = (iwrphase - idsamp) & mask;
            irdphaseb = (irdphase - 1) & mask;
            if (numoutput < idelaylen) {
                if (irdphase > iwrphase) {
                    // value += 0.f;
                } else if (irdphaseb > iwrphase) {
                    d1 = dlybuf[irdphase];
                    value += (d1 - frac * d1) * ramp2;
                } else {
                    d1 = dlybuf[irdphase];
                    d2 = dlybuf[irdphaseb];
                    value += (d1 + frac * (d2 - d1)) * ramp2;
                }
            } else {
                d1 = dlybuf[irdphase];
                d2 = dlybuf[irdphaseb];
                value += (d1 + frac * (d2 - d1)) * ramp2;
            }
            ramp2 += ramp2_slope;

            dsamp3 += dsamp3_slope;
            idsamp = (long)dsamp3;
            frac = dsamp3 - idsamp;
            irdphase = (iwrphase - idsamp) & mask;
            irdphaseb = (irdphase - 1) & mask;
            if (numoutput < idelaylen) {
                if (irdphase > iwrphase) {
                    // value += 0.f;
                } else if (irdphaseb > iwrphase) {
                    d1 = dlybuf[irdphase];
                    value += (d1 - frac * d1) * ramp3;
                } else {
                    d1 = dlybuf[irdphase];
                    d2 = dlybuf[irdphaseb];
                    value += (d1 + frac * (d2 - d1)) * ramp3;
                }
            } else {
                d1 = dlybuf[irdphase];
                d2 = dlybuf[irdphaseb];
                value += (d1 + frac * (d2 - d1)) * ramp3;
            }
            ramp3 += ramp3_slope;

            dsamp4 += dsamp4_slope;
            idsamp = (long)dsamp4;
            frac = dsamp4 - idsamp;
            irdphase = (iwrphase - idsamp) & mask;
            irdphaseb = (irdphase - 1) & mask;

            if (numoutput < idelaylen) {
                if (irdphase > iwrphase) {
                    // value += 0.f;
                } else if (irdphaseb > iwrphase) {
                    d1 = dlybuf[irdphase];
                    value += (d1 - frac * d1) * ramp4;
                } else {
                    d1 = dlybuf[irdphase];
                    d2 = dlybuf[irdphaseb];
                    value += (d1 + frac * (d2 - d1)) * ramp4;
                }
            } else {
                d1 = dlybuf[irdphase];
                d2 = dlybuf[irdphaseb];
                value += (d1 + frac * (d2 - d1)) * ramp4;
            }
            ramp4 += ramp4_slope;

            dlybuf[iwrphase] = *in++/*ZXP(in)*/;
            *out++/*ZXP(out)*/ = value *= 0.5;
        }
    }

    unit->counter = counter;
    unit->stage = stage;
    unit->mask = mask;

    unit->dsamp1 = dsamp1;
    unit->dsamp2 = dsamp2;
    unit->dsamp3 = dsamp3;
    unit->dsamp4 = dsamp4;

    unit->ramp1 = ramp1;
    unit->ramp2 = ramp2;
    unit->ramp3 = ramp3;
    unit->ramp4 = ramp4;

    unit->numoutput = numoutput;
    unit->iwrphase = iwrphase;

    if (numoutput >= idelaylen) {
        calc_ = &GdShifter::processNext/*(SETCALC(PitchShift_next)*/;
    }

    rgen_.s1 = s1; rgen_.s2 = s2; rgen_.s3 = s3;/*RPUT*/
}

void GdShifter::copyNext(const float *input, float *output, unsigned count)
{
    if (input != output)
        std::memcpy(output, input, count * sizeof(float));
}
