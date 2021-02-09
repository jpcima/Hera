// SPDX-License-Identifier: GPL-3.0-or-later

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cstdio>

#if defined(JucePlugin_PreferredChannelConfigurations) || \
    JucePlugin_IsMidiEffect || !JucePlugin_IsSynth || \
    !JucePlugin_WantsMidiInput || JucePlugin_ProducesMidiOutput
#error Bad processor parameters
#endif

//==============================================================================
HeraAudioProcessor::HeraAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      state(*this, nullptr, "PARAMETERS", synthesizer.makeParameterLayout()),
      magicState(*this, state)
{
    addListener(this);
    setCurrentProgram(0);
}

HeraAudioProcessor::~HeraAudioProcessor()
{
}

//==============================================================================
const juce::String HeraAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HeraAudioProcessor::acceptsMidi() const
{
    return true;
}

bool HeraAudioProcessor::producesMidi() const
{
    return false;
}

bool HeraAudioProcessor::isMidiEffect() const
{
    return false;
}

double HeraAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HeraAudioProcessor::getNumPrograms()
{
    return kHeraNumPrograms;
}

int HeraAudioProcessor::getCurrentProgram()
{
    return currentProgramNumber;
}

void HeraAudioProcessor::setCurrentProgram(int index)
{
    jassert(index >= 0 && index < kHeraNumPrograms);
    currentProgramNumber = index;
    flushCurrentProgramToParameters();
}

const juce::String HeraAudioProcessor::getProgramName(int index)
{
    jassert(index >= 0 && index < kHeraNumPrograms);
    return bank.program[index].name;
}

void HeraAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    jassert(index >= 0 && index < kHeraNumPrograms);
    bank.program[index].setName(newName);
}

//==============================================================================
void HeraAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synthesizer.setCurrentPlaybackSampleRate(HERA_OVERSAMPLING * sampleRate);

#if HERA_OVERSAMPLING > 1
    osMidiMessages.ensureSize(2048);
    os.clear();
#endif

    ensureBlockSize(samplesPerBlock);
}

void HeraAudioProcessor::releaseResources()
{
}

bool HeraAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void HeraAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                      juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int numSamples = buffer.getNumSamples();

    // never happens if the host sends correct block size
    ensureBlockSize(numSamples);

    juce::AudioPlayHead::CurrentPositionInfo positionInfo;
    if (getPlayHead()->getCurrentPosition(positionInfo))
        synthesizer.setTempo(positionInfo.bpm);

#if HERA_OVERSAMPLING > 1
    // shift all frame times according to the oversampling factor
    osMidiMessages.clear();
    for (juce::MidiMessageMetadata message : midiMessages)
        osMidiMessages.addEvent(message.data, message.numBytes, HERA_OVERSAMPLING * message.samplePosition);

    // generate oversampled
    osStereoBuffer.clear(0, HERA_OVERSAMPLING * numSamples);
    synthesizer.renderNextBlockUsingArpeggiator(osStereoBuffer, osMidiMessages, 0, HERA_OVERSAMPLING * numSamples);

    // downsample
    os.downsample(osStereoBuffer.getArrayOfReadPointers(), buffer.getArrayOfWritePointers(), numSamples);
#else
    synthesizer.renderNextBlockUsingArpeggiator(buffer, midiMessages, 0, numSamples);
#endif
}

//==============================================================================
bool HeraAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor *HeraAudioProcessor::createEditor()
{
    return new HeraAudioProcessorEditor(magicState);
}

//==============================================================================
void HeraAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::MemoryOutputStream memStream(destData, false);
    juce::GZIPCompressorOutputStream gzipStream(memStream);
    juce::ValueTree tree("BANK");
    tree.setProperty("current", currentProgramNumber, nullptr);
    const HeraProgram initProgram = Bank::makeInitProgram();
    for (int pgm = 0; pgm < kHeraNumPrograms; ++pgm) {
        const HeraProgram &program = bank.program[pgm];
        if (program == initProgram)
            continue;
        juce::ValueTree programTree = program.toValueTree();
        programTree.setProperty("index", pgm, nullptr);
        tree.addChild(programTree, -1, nullptr);
    }
    tree.writeToStream(gzipStream);
}

void HeraAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromGZIPData(data, sizeInBytes);
    if (!tree.isValid())
        return;
    currentProgramNumber = juce::jlimit(0, kHeraNumPrograms - 1, (int)tree.getProperty("current"));
    if (currentProgramNumber < 0 || currentProgramNumber >= kHeraNumPrograms)
        currentProgramNumber = 0;
    const HeraProgram initProgram = Bank::makeInitProgram();
    for (int pgm = 0; pgm < kHeraNumPrograms; ++pgm)
        bank.program[pgm] = initProgram;
    for (int i = 0, n = tree.getNumChildren(); i < n; ++i) {
        juce::ValueTree programTree = tree.getChild(i);
        int pgm = (int)programTree.getProperty("index");
        if (pgm < 0 || pgm >= kHeraNumPrograms)
            continue;
        bank.program[pgm] = HeraProgram::fromValueTree(programTree);
    }
    flushCurrentProgramToParameters();
}

void HeraAudioProcessor::getCurrentProgramStateInformation(juce::MemoryBlock &destData)
{
    juce::MemoryOutputStream memStream(destData, false);
    juce::GZIPCompressorOutputStream gzipStream(memStream);
    juce::ValueTree tree = bank.program[currentProgramNumber].toValueTree();
    tree.writeToStream(gzipStream);
}

void HeraAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromGZIPData(data, sizeInBytes);
    if (!tree.isValid())
        return;
    bank.program[currentProgramNumber] = HeraProgram::fromValueTree(tree);
    flushCurrentProgramToParameters();
}

void HeraAudioProcessor::audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue)
{
    juce::ignoreUnused(processor);
    HeraProgram &program = bank.program[currentProgramNumber];
    auto *param = static_cast<juce::RangedAudioParameter *>(getParameters()[parameterIndex]);
    program.values[parameterIndex] = param->convertFrom0to1(newValue);
}

void HeraAudioProcessor::flushCurrentProgramToParameters()
{
    const auto &parameters = getParameters();
    const HeraProgram &program = bank.program[currentProgramNumber];
    for (int p = 0; p < kHeraNumParameters; ++p) {
        auto *param = static_cast<juce::RangedAudioParameter *>(parameters[p]);
        param->setValueNotifyingHost(param->convertTo0to1(program.values[p]));
    }
}

void HeraAudioProcessor::ensureBlockSize(int blockSize)
{
    if (reservedBlockSize >= blockSize)
        return;

    blockSize = (blockSize < 2048) ? 2048 : juce::nextPowerOfTwo(blockSize);

#if HERA_OVERSAMPLING > 1
    osStereoBuffer = juce::AudioBuffer<float>(2, HERA_OVERSAMPLING * blockSize);
#endif

    synthesizer.reserveBlockSize(HERA_OVERSAMPLING * blockSize);

    reservedBlockSize = blockSize;
}

//==============================================================================
#if HERA_OVERSAMPLING > 1
static constexpr double OSCoeffs2x[12] = { 0.036681502163648017, 0.13654762463195794, 0.27463175937945444, 0.42313861743656711, 0.56109869787919531, 0.67754004997416184, 0.76974183386322703, 0.83988962484963892, 0.89226081800387902, 0.9315419599631839, 0.96209454837808417, 0.98781637073289585 };
#endif

#if HERA_OVERSAMPLING == 2
HeraAudioProcessor::Oversampler::Oversampler()
{
    for (int c = 0; c < 2; ++c)
        downsampler2x[c].set_coefs(OSCoeffs2x);
}

void HeraAudioProcessor::Oversampler::clear()
{
    for (int c = 0; c < 2; ++c)
        downsampler2x[c].clear_buffers();
}

void HeraAudioProcessor::Oversampler::downsample(const float *const *inputs, float *const *outputs, int numOutputSamples)
{
    for (int c = 0; c < 2; ++c)
        downsampler2x[c].process_block(outputs[c], inputs[c], numOutputSamples);
}
#endif

//==============================================================================
HeraAudioProcessor::Bank::Bank()
{
    const auto &presets = HeraProgram::getPresets();
    int numPresets = presets.size();

    for (int i = 0; i < numPresets && i < kHeraNumPrograms; ++i)
        program[i] = presets[i];

    const HeraProgram initProgram = makeInitProgram();
    for (int i = numPresets; i < kHeraNumPrograms; ++i)
        program[i] = initProgram;
}

HeraProgram HeraAudioProcessor::Bank::makeInitProgram()
{
    const auto &presets = HeraProgram::getPresets();
    HeraProgram initProgram = presets[0];
    initProgram.setName("");
    return initProgram;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new HeraAudioProcessor;
}
