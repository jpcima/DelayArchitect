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

#include "ImporterPST.h"
#include <Gd.h>

bool ImporterPST::importFile(const juce::File &file, ImportData &idata)
{
    juce::FileInputStream stream(file);
    if (!stream.openedOk())
        return false;

    if (!stream.setPosition(0x0c))
        return false;

    auto fourcc = [](const char *s) -> uint32_t {
        return (uint32_t)(uint8_t)s[3] | (uint32_t)((uint8_t)s[2] << 8) |
            (uint32_t)((uint8_t)s[1] << 16) | (uint32_t)((uint8_t)s[0] << 24);
    };

    bool big;
    uint32_t fileType = (uint32_t)stream.readInt();
    if (fileType == fourcc("EMAG"))
        big = false;
    else if (fileType == fourcc("GAME"))
        big = true;
    else
        return false;

    auto readU32 = [&stream, big]() -> uint32_t {
        return (uint32_t)(big ? stream.readIntBigEndian() : stream.readInt());
    };
    auto readF32 = [&stream, big]() -> float {
        return big ? stream.readFloatBigEndian() : stream.readFloat();
    };

    uint32_t fileSubtype = readU32();
    if (fileSubtype != fourcc("PPST"))
        return false;

    ///
    PresetFile &pst = idata.pst;
    pst.valid = true;
    for (uint32_t p = 0; p < GD_PARAMETER_COUNT; ++p)
        pst.values[p] = GdParameterDefault((GdParameter)p);

    ///
    {
        float *globalValues = pst.values;
        for (uint32_t i = 0, end = false; !end; ++i) {
            float value = readF32();
            if (stream.isExhausted())
                break;
            switch (0x14 + 4 * i) {
            case 0x14: // ??
                break;
            case 0x18: // Sync (boolean) [float32]
                globalValues[GDP_SYNC] = (bool)value;
                break;
            case 0x1c: // Grid (real number 0<Div<1) [float32]
                globalValues[GDP_GRID] = (float)(int)(1.0f / value + 0.5f);
                break;
            case 0x20: // Swing (%) [float32]
                globalValues[GDP_SWING] = value;
                break;
            case 0x24: // Feedback (boolean) [float32]
                globalValues[GDP_FEEDBACK_ENABLE] = (bool)value;
                break;
            case 0x28: // Feedback tap (0-based) [float32]
                globalValues[GDP_FEEDBACK_TAP] = (float)value;
                break;
            case 0x2c: // Feedback gain (dB) [float32]
                globalValues[GDP_FEEDBACK_GAIN] = value;
                break;
            case 0x30: // Dry gain (dB) [float32]
                globalValues[GDP_MIX_DRY] = value;
                break;
            case 0x34: // Wet gain (dB) [float32]
                globalValues[GDP_MIX_WET] = value;
                break;
            default:
                end = true;
                break;
            }
        }
    }

    if (!stream.setPosition(0x4c))
        return false;

    bool haveBlock = false;
    uint32_t blockMagic;
    uint32_t blockSize;
    while (blockMagic = readU32(), blockSize = readU32(), !stream.isExhausted()) {
        int64_t blockPosition = stream.getPosition() - 8;

        int tapNumber;
        if ((blockMagic & 0xffffff00u) == fourcc("Tap\0") &&
            (tapNumber = (int)(blockMagic & 0xff) - 'A') >= 0 && tapNumber < GdMaxLines)
        {
            float *tapValues = &pst.values[tapNumber * GdNumPametersPerTap];
            tapValues[GDP_TAP_A_ENABLE] = 1.0f;

            uint32_t numValues = blockSize / sizeof(float);
            for (uint32_t i = 0, end = false; i < numValues && !end; ++i) {
                float value = readF32();
                if (stream.isExhausted())
                    break;
                switch (0x08 + 4 * i) {
                case 0x08: // Delay (ms) [float32]
                    tapValues[GDP_TAP_A_DELAY] = value / 1000.0f;
                    break;
                case 0x0c: // Unique ID? (1-based) [float32]
                    break;
                case 0x10: // Level (dB) [float32]
                    tapValues[GDP_TAP_A_LEVEL] = value;
                    break;
                case 0x14: // Mute (boolean) [float32]
                    tapValues[GDP_TAP_A_MUTE] = (bool)value;
                    break;
                case 0x18: // Pan (%) [float32]
                    tapValues[GDP_TAP_A_PAN] = value;
                    break;
                case 0x1c: // Spread (%) [float32]
                    tapValues[GDP_TAP_A_WIDTH] = value;
                    break;
                case 0x20: // ?? [float32]
                    break;
                case 0x24: // ?? [float32]
                    break;
                case 0x28: // ?? [float32]
                    break;
                case 0x2c: // ?? [float32]
                    break;
                case 0x30: // ?? [float32]
                    break;
                case 0x34: // Flip (boolean) [float32]
                    tapValues[GDP_TAP_A_FLIP] = (bool)value;
                    break;
                case 0x38: // ?? [float32]
                    break;
                case 0x3c: // Highpass cutoff (Hz) [float32]
                    tapValues[GDP_TAP_A_HPF_CUTOFF] = value;
                    break;
                case 0x40: // Lowpass cutoff (Hz) [float32]
                    tapValues[GDP_TAP_A_LPF_CUTOFF] = value;
                    break;
                case 0x44: // Filter enabled (boolean) [float32]
                    tapValues[GDP_TAP_A_FILTER_ENABLE] = (bool)value;
                    break;
                case 0x48: // Resonance (%) [float32]
                    tapValues[GDP_TAP_A_RESONANCE] = value * (20.0f / 100.0f);
                    break;
                case 0x4c: // Filter slope (0=6dB/oct, 1=12dB/oct) [float32]
                    tapValues[GDP_TAP_A_FILTER] = (float)(int)value;
                    break;
                case 0x50: // Transpose (semitones) [float32]
                    tapValues[GDP_TAP_A_TUNE] = value * 100.0f;
                    break;
                case 0x54: // Pitch enabled (boolean) [float32]
                    tapValues[GDP_TAP_A_TUNE_ENABLE] = (bool)value;
                    break;
                default:
                    end = true;
                    break;
                }
            }

            haveBlock = true;
        }

        if (!stream.setPosition(blockPosition + blockSize))
            return false;
    }

    if (!haveBlock)
        return false;

    for (uint32_t p = 0; p < GD_PARAMETER_COUNT; ++p) {
        float min = GdParameterMin((GdParameter)p);
        float max = GdParameterMax((GdParameter)p);
        pst.values[p] = juce::jlimit(min, max, pst.values[p]);
    }

    return true;
}
