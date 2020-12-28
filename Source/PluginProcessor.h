// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "Hera.h"
#include "HeraProgram.h"
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <foleys_gui_magic/foleys_gui_magic.h>
#include <hiir/Downsampler2xFpu.h>

//==============================================================================
/**
 */
class HeraAudioProcessor : public juce::AudioProcessor,
                           public juce::AudioProcessorListener {
public:
    //==============================================================================
    HeraAudioProcessor();
    ~HeraAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    //==============================================================================
    void audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue) override;
    void audioProcessorChanged(AudioProcessor *processor) override {}
    void getCurrentProgramStateInformation(juce::MemoryBlock &destData) override;
    void setCurrentProgramStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void flushCurrentProgramToParameters();

    //==============================================================================
    enum { kHeraNumPrograms = 128 };
    void fillInitialBank();

private:
    void ensureBlockSize(int blockSize);

private:
    HeraSynthesiser synthesizer;

    int currentProgramNumber = -1;
    struct Bank {
        Bank();
        HeraProgram program[kHeraNumPrograms];
        static HeraProgram makeInitProgram();
    } bank;

#if HERA_OVERSAMPLING > 1
    struct Oversampler {
        Oversampler();
        void clear();
        void downsample(const float *const *inputs, float *const *outputs, int numOutputSamples);
#if HERA_OVERSAMPLING == 2
        hiir::Downsampler2xFpu<12> downsampler2x[2];
#endif
    };
    Oversampler os;
    juce::MidiBuffer osMidiMessages;
    juce::AudioBuffer<float> osStereoBuffer;
#endif

    int reservedBlockSize = 0;

    juce::AudioProcessorValueTreeState state;
    foleys::MagicProcessorState magicState;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeraAudioProcessor)
};
