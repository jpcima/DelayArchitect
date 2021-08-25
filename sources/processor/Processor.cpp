// SPDX-License-Identifier: BSD-2-Clause
#include "processor/Processor.h"
#include "editor/Editor.h"
#include "Gd.h"

struct Processor::Impl : public juce::AudioProcessorListener {
    explicit Impl(Processor *self) : self_(self) {}
    void setupParameters();

    //==========================================================================
    void updateBPM(double newBpm);

    //==========================================================================
    void audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue) override;
    void audioProcessorChanged (AudioProcessor *processor, const ChangeDetails &details) override;

    //==========================================================================
    Processor *self_ = nullptr;
    GdPtr gd_;
    double lastKnownBpm_ = -1.0;
};

//==============================================================================
Processor::Processor()
    : impl_(new Impl(this))
{
    Impl &impl = *impl_;
    addListener(&impl);
    impl.setupParameters();
}

Processor::~Processor()
{
    Impl &impl = *impl_;
    removeListener(&impl);
}

//==============================================================================
void Processor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    Impl &impl = *impl_;
    Gd *gd = impl.gd_.get();

    if (!gd) {
        BusesLayout layouts = getBusesLayout();
        juce::AudioChannelSet inputs = layouts.getMainInputChannelSet();
        int numInputs = (inputs == juce::AudioChannelSet::stereo()) ? 2 : 1;
        int numOutputs = 2;
        gd = GdNew((unsigned)numInputs, (unsigned)numOutputs);
        jassert(gd);
        impl.gd_.reset(gd);
    }

    GdSetSampleRate(gd, (float)sampleRate);
    GdSetBufferSize(gd, (unsigned)samplesPerBlock);
    GdSetTempo(gd, 120.0f);

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        const auto &parameter = static_cast<const juce::RangedAudioParameter &>(*getParameters()[(int)i]);
        float value = parameter.convertFrom0to1(parameter.getValue());
        GdSetParameter(gd, (GdParameter)i, value);
    }

    GdClear(gd);

    impl.lastKnownBpm_ = -1.0;
}

void Processor::releaseResources()
{
    Impl &impl = *impl_;
    impl.gd_.reset();
}

bool Processor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    juce::AudioChannelSet inputs = layouts.getMainInputChannelSet();
    juce::AudioChannelSet outputs = layouts.getMainOutputChannelSet();

    return (inputs == juce::AudioChannelSet::mono() ||
            inputs == juce::AudioChannelSet::stereo()) &&
        outputs == juce::AudioChannelSet::stereo();
}

bool Processor::applyBusLayouts(const BusesLayout& layouts)
{
    if (layouts == getBusesLayout())
        return true;

    if (!AudioProcessor::applyBusLayouts(layouts))
        return false;

    Impl &impl = *impl_;
    if (impl.gd_) {
        impl.gd_.reset();
        prepareToPlay(getSampleRate(), getBlockSize());
    }

    return true;
}

void Processor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    (void)midiMessages;

    Impl &impl = *impl_;

    ///
    juce::AudioPlayHead *playHead = getPlayHead();
    juce::AudioPlayHead::CurrentPositionInfo playPosition;
    if (playHead->getCurrentPosition(playPosition))
        impl.updateBPM(playPosition.bpm);

    ///
    Gd *gd = impl.gd_.get();
    const float **inputs = buffer.getArrayOfReadPointers();
    float **outputs = buffer.getArrayOfWritePointers();
    GdProcess(gd, inputs, outputs, (unsigned)buffer.getNumSamples());
}

void Processor::processBlock(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiMessages)
{
    (void)buffer;
    (void)midiMessages;
    jassertfalse;
}

//==============================================================================
juce::AudioProcessorEditor *Processor::createEditor()
{
    return new Editor(*this);
}

bool Processor::hasEditor() const
{
    return true;
}

//==============================================================================
const juce::String Processor::getName() const
{
    return JucePlugin_Name;
}

bool Processor::acceptsMidi() const
{
    return false;
}

bool Processor::producesMidi() const
{
    return false;
}

bool Processor::isMidiEffect() const
{
    return false;
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
int Processor::getNumPrograms()
{
    return 1;
}

int Processor::getCurrentProgram()
{
    return 0;
}

void Processor::setCurrentProgram(int index)
{
    (void)index;
}

const juce::String Processor::getProgramName(int index)
{
    (void)index;
    return {};
}

void Processor::changeProgramName(int index, const juce::String &newName)
{
    (void)index;
    (void)newName;
}

//==============================================================================
static const char StateIdentifier_v1[] = "DelayArchitectV1";

void Processor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::ValueTree tree(StateIdentifier_v1);

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        const auto &parameter = static_cast<const juce::RangedAudioParameter &>(*getParameters()[(int)i]);
        const char *name = GdParameterName((GdParameter)i);
        float value = parameter.convertFrom0to1(parameter.getValue());
        tree.setProperty(name, (double)value, nullptr);
    }


    juce::MemoryOutputStream memStream(destData, false);
    juce::GZIPCompressorOutputStream gzipStream(memStream);
    tree.writeToStream(gzipStream);
}

void Processor::setStateInformation(const void *data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromGZIPData(data, (size_t)sizeInBytes);
    if (!tree.isValid() || tree.getType() != juce::StringRef(StateIdentifier_v1))
        return;

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        auto &parameter = static_cast<juce::RangedAudioParameter &>(*getParameters()[(int)i]);
        const char *name = GdParameterName((GdParameter)i);
        juce::var value = tree.getProperty(name);
        if (!value.isDouble())
            value = (double)GdParameterDefault((GdParameter)i);
        parameter.setValueNotifyingHost(parameter.convertTo0to1((float)(double)value));
    }
}

//==============================================================================
void Processor::Impl::setupParameters()
{
    Processor *self = self_;

    std::vector<std::unique_ptr<juce::AudioProcessorParameterGroup>> parameterGroups;
    juce::AudioProcessorParameterGroup *lastPg = nullptr;

    parameterGroups.reserve(32);

    for (unsigned i = 0; i < GD_PARAMETER_COUNT; ++i) {
        unsigned flags = GdParameterFlags((GdParameter)i);
        const char *name = GdParameterName((GdParameter)i);
        const char *label = GdParameterLabel((GdParameter)i);
        float min = GdParameterMin((GdParameter)i);
        float max = GdParameterMax((GdParameter)i);
        float def = GdParameterDefault((GdParameter)i);
        int group = GdParameterGroup((GdParameter)i);
        unsigned type = flags & (GDP_FLOAT|GDP_BOOLEAN|GDP_INTEGER|GDP_CHOICE);

        ///
        int previousGroup = (int)parameterGroups.size() - 1;
        if (group != previousGroup) {
            jassert(group == previousGroup + 1);
            const char *groupName = GdGroupName((GdParameter)i);
            const char *groupLabel = GdGroupLabel((GdParameter)i);
            lastPg = new juce::AudioProcessorParameterGroup(groupName, groupLabel, "|");
            parameterGroups.emplace_back(lastPg);
        }

        ///
        juce::AudioProcessorParameter *parameter;

        auto stringFromValue = [i](float value, int) -> juce::String {
            char text[256];
            GdFormatParameterValue((GdParameter)i, value, text, sizeof(text));
            return text;
        };

        switch (type) {
        default:
        case GDP_FLOAT:
        {
            juce::AudioParameterFloat *parameterFloat = new juce::AudioParameterFloat(name, label, {min, max}, def, juce::String{}, juce::AudioProcessorParameter::genericParameter, stringFromValue);
            parameter = parameterFloat;
            break;
        }
        case GDP_BOOLEAN:
            parameter = new juce::AudioParameterBool(name, label, (bool)def, juce::String{}, stringFromValue);
            break;
        case GDP_INTEGER:
        {
            parameter = new juce::AudioParameterInt(name, label, (int)min, (int)max, (int)def, juce::String{}, stringFromValue);
            break;
        }
        case GDP_CHOICE:
            {
                juce::StringArray choices;
                choices.ensureStorageAllocated(32);
                for (const char *const *p = GdParameterChoices((GdParameter)i); *p; ++p)
                    choices.add(*p);
                parameter = new juce::AudioParameterChoice(name, label, std::move(choices), (int)def, juce::String{}, stringFromValue);
            }
            break;
        }

        if (!lastPg)
            self->addParameter(parameter);
        else
            lastPg->addChild(std::unique_ptr<juce::AudioProcessorParameter>(parameter));
    }

    for (int i = 0, n = (int)parameterGroups.size(); i < n; ++i) {
        self->addParameterGroup(std::move(parameterGroups[(size_t)i]));
    }
}

//==============================================================================
void Processor::Impl::updateBPM(double newBpm)
{
    Processor *self = self_;

    double oldBpm = lastKnownBpm_;
    if (oldBpm == newBpm)
        return;
    lastKnownBpm_ = newBpm;

    Gd *gd = gd_.get();
    GdSetTempo(gd, (float)newBpm);

    if (oldBpm != -1.0) {
        float scaleRatio = (float)(oldBpm / newBpm);
        juce::AudioParameterBool *sync = static_cast<juce::AudioParameterBool *>(self->getParameters()[(int)GDP_SYNC]);
        if (*sync) {
            for (int tapNumber = 0; tapNumber < GdMaxLines; ++tapNumber) {
                GdParameter delayId = GdRecomposeParameter(GDP_TAP_A_DELAY, tapNumber);
                juce::AudioParameterFloat *delay = static_cast<juce::AudioParameterFloat *>(self->getParameters()[(int)delayId]);
                *delay = scaleRatio * *delay;
            }
        }
    }
}

//==============================================================================
void Processor::Impl::audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue)
{
    (void)processor;

    Processor *self = self_;
    const auto &parameter = static_cast<const juce::RangedAudioParameter &>(*self->getParameters()[(int)parameterIndex]);
    newValue = parameter.convertFrom0to1(newValue);

    Gd *gd = gd_.get();
    GdSetParameter(gd, (GdParameter)parameterIndex, newValue);
}

void Processor::Impl::audioProcessorChanged(AudioProcessor *processor, const ChangeDetails &details)
{
    (void)processor;
    (void)details;
}

//==============================================================================
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new Processor;
}
