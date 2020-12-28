// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "HeraEnvelope.h"
#include "HeraLFOWithEnvelope.h"
#include "HeraDCO.hxx"
#include "LadderFilter.h"
#include "HeraHPF.hxx"
#include "HeraVCA.hxx"
#include "HeraChorus.hxx"
#include "LerpTable.h"
#include "SmoothValue.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>

#define HERA_OVERSAMPLING 2

enum {
    kHeraMaxVoices = 6,
};

enum {
    kHeraParamVCA,
    kHeraParamVCAType,
    kHeraParamPWMDepth,
    kHeraParamPWMMod,
    kHeraParamSawLevel,
    kHeraParamPulseLevel,
    kHeraParamSubLevel,
    kHeraParamNoiseLevel,
    kHeraParamPitchRange,
    kHeraParamPitchModDepth,
    kHeraParamVCFCutoff,
    kHeraParamVCFResonance,
    kHeraParamVCFEnvelopePolarity,
    kHeraParamVCFEnvelopeModDepth,
    kHeraParamVCFLFOModDepth,
    kHeraParamVCFKeyboardModDepth,
    kHeraParamVCFBendDepth,
    kHeraParamAttack,
    kHeraParamDecay,
    kHeraParamSustain,
    kHeraParamRelease,
    kHeraParamLFOTriggerMode,
    kHeraParamLFORate,
    kHeraParamLFODelay,
    kHeraParamHPF,
    kHeraParamChorusI,
    kHeraParamChorusII,
    kHeraNumParameters,
};

enum {
    kHeraVCATypeEnvelope,
    kHeraVCATypeGate,
};

enum {
    kHeraPWMManual,
    kHeraPWMLFO,
    kHeraPWMEnvelope,
};

enum {
    kHeraLFOManual,
    kHeraLFOAuto,
};

enum {
    kHeraVCFEnvelopeNegative,
    kHeraVCFEnvelopePositive,
};

enum HeraParameterDomain {
    kHeraParameterIsForSynth,
    kHeraParameterIsForVoice,
};

class HeraVoice;

class HeraSynthesiser : public juce::MPESynthesiser,
                        public juce::AudioProcessorParameter::Listener {
public:
    HeraSynthesiser();
    void setupParameters();
    juce::AudioProcessorValueTreeState::ParameterLayout makeParameterLayout();
    void setCurrentPlaybackSampleRate(double newRate) override;
    void reserveBlockSize(int blockSize);
    void renderNextSubBlock(juce::AudioBuffer<float> &outputAudio, int startSample, int numSamples) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

    bool hasUnreleasedVoices() const;

    static juce::RangedAudioParameter *createParameter(int index, HeraParameterDomain &domain);

    static const juce::OwnedArray<juce::RangedAudioParameter> &getReferenceParameters();

private:
    void processDetune(int numSamples);
    void processCutoffAndResonance(int numSamples);

private:
    const LerpTable &getSoftClipFunction();
    void softClip(juce::AudioBuffer<float> &buffer, int startSample, int numSamples);

private:
    HeraLFOWithEnvelope lfo;
    juce::AudioBuffer<float> lfoBuffer;
    juce::AudioBuffer<float> detuneBuffer;
    int vcaType = kHeraVCATypeEnvelope;
    float pitchFactor = 1;
    OnePoleSmoothValue smoothPitchModDepth;
    OnePoleSmoothValue smoothCutoff { 1.0f };
    OnePoleSmoothValue smoothResonance;
    int vcfEnvPolarity = kHeraVCFEnvelopeNegative;
    OnePoleSmoothValue smoothVCFEnvModDepth;
    OnePoleSmoothValue smoothVCFLFOModDepth;
    OnePoleSmoothValue smoothVCFKeyboardModDepth;
    OnePoleSmoothValue smoothVCFBendDepth;
    int lfoMode = kHeraLFOManual;
    HeraHPF hpFilter;
    HeraVCA vca;
    HeraChorus chorus;

    bool parametersReady = false;
    juce::OptionalScopedPointer<juce::RangedAudioParameter> parameters[kHeraNumParameters] = {};

    // shared voice data
    juce::AudioBuffer<float> dcoBuffer;
    juce::AudioBuffer<float> envelopeBuffer;
    juce::AudioBuffer<float> gateBuffer;
    juce::AudioBuffer<float> pwmModBuffer;
    juce::AudioBuffer<float> cutoffOctavesBuffer;
    juce::AudioBuffer<float> cutoffBuffer;
    juce::AudioBuffer<float> resonanceBuffer;
    juce::AudioBuffer<float> vcfEnvModBuffer;
    juce::AudioBuffer<float> vcfLFODetuneOctavesBuffer;
    juce::AudioBuffer<float> vcfKeyboardModBuffer;
    juce::AudioBuffer<float> vcfBendDepthBuffer;

private:
    friend class HeraVoice;
};

class HeraVoice : public juce::MPESynthesiserVoice,
                  public juce::AudioProcessorParameter::Listener {
public:
    explicit HeraVoice(HeraSynthesiser *synthesiser);
    void setCurrentSampleRate(double newRate) override;
    void reserveBlockSize(int blockSize);
    void noteStarted() override;
    void noteStopped(bool allowTailOff) override;
    void notePressureChanged() override;
    void notePitchbendChanged() override;
    void noteTimbreChanged() override;
    void noteKeyStateChanged() override;
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

    const HeraEnvelope &getCurrentEnvelope() const
    {
        return (vcaType == kHeraVCATypeEnvelope) ? normalEnvelope : gateEnvelope;
    }
    HeraEnvelope &getCurrentEnvelope()
    {
        return const_cast<HeraEnvelope &>(
            const_cast<const HeraVoice *>(this)->getCurrentEnvelope());
    }

private:
    void processEnvelope(int numSamples);
    void processPWM(int numSamples);
    void processDCO(int numSamples);
    void processVCF(int numSamples);
    void processMix(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples);

private:
    HeraSynthesiser *synthesiser = nullptr;
    int vcaType = kHeraVCATypeEnvelope;
    HeraDCO dco;
    LadderFilter vcf;
    HeraEnvelope normalEnvelope;
    HeraEnvelope gateEnvelope;
    OnePoleSmoothValue smoothPWMDepth;
    int pwmMod = kHeraPWMManual;
};
