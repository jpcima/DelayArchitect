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

#include "PresetFile.h"
#include "Gd.h"
#include <juce_data_structures/juce_data_structures.h>

static const char StateIdentifier_v1[] = "DelayArchitectV1";

PresetFile PresetFile::makeDefault()
{
    PresetFile pst;

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i)
        pst.values[i] = GdParameterDefault((GdParameter)i);

    pst.valid = true;
    return pst;
}

PresetFile PresetFile::loadFromStream(juce::InputStream &stream)
{
    PresetFile pst = makeDefault();
    pst.valid = false;

    juce::GZIPDecompressorInputStream gzipStream(&stream, false);
    juce::ValueTree tree = juce::ValueTree::readFromStream(gzipStream);
    if (!tree.isValid() || tree.getType() != juce::StringRef(StateIdentifier_v1))
        return pst;

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        const char *name = GdParameterName((GdParameter)i);
        juce::var value = tree.getProperty(name);
        if (value.isDouble())
            pst.values[i] = (float)(double)value;
    }

    pst.valid = true;
    return pst;
}

PresetFile PresetFile::loadFromData(const void *data, size_t numBytes)
{
    juce::MemoryInputStream stream(data, numBytes, false);
    return loadFromStream(stream);
}

PresetFile PresetFile::loadFromFile(const juce::File &file)
{
    PresetFile pst;
    juce::FileInputStream stream(file);
    if (stream.failedToOpen())
        return pst;
    pst = loadFromStream(stream);
    if (!stream.getStatus().wasOk())
        pst.valid = false;
    return pst;
}

bool PresetFile::saveToStream(const PresetFile &pst, juce::OutputStream &stream)
{
    if (!pst)
        return false;

    juce::ValueTree tree(StateIdentifier_v1);

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        const char *name = GdParameterName((GdParameter)i);
        float value = pst.values[i];
        tree.setProperty(name, (double)value, nullptr);
    }

    juce::GZIPCompressorOutputStream gzipStream(stream);
    tree.writeToStream(gzipStream);

    return true;
}

bool PresetFile::saveToData(const PresetFile &pst, juce::MemoryBlock &data, bool appendData)
{
    juce::MemoryOutputStream stream(data, appendData);
    return saveToStream(pst, stream);
}

bool PresetFile::saveToFile(const PresetFile &pst, const juce::File &file)
{
    std::unique_ptr<juce::FileOutputStream> stream(new juce::FileOutputStream(file));
    if (!stream->openedOk())
        return false;

    stream->setPosition(0);
    stream->truncate();
    if (!saveToStream(pst, *stream)) {
        stream.reset();
        file.deleteFile();
        return false;
    }

    stream->flush();
    if (!stream->getStatus().wasOk()) {
        stream.reset();
        file.deleteFile();
        return false;
    }

    return true;
}
