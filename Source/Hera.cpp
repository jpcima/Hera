#include "Hera.h"
#include "HeraTables.h"
#include "FaustHelpers.h"
#include <juce_dsp/juce_dsp.h>
#include <cmath>

HeraSynthesiser::HeraSynthesiser()
{
    setVoiceStealingEnabled(true);
    setMinimumRenderingSubdivisionSize(16);

    for (int i = 0; i < kHeraMaxVoices; ++i) {
        HeraVoice *voice = new HeraVoice(this);
        addVoice(voice);
    }

    lfo.setType(HeraLFO::Sine);
    smoothPitchModDepth.setTimeConstant(10e-3f);
    smoothCutoff.setTimeConstant(10e-3f);
    smoothResonance.setTimeConstant(10e-3f);
    smoothVCFEnvModDepth.setTimeConstant(10e-3f);
    smoothVCFLFOModDepth.setTimeConstant(10e-3f);
    smoothVCFKeyboardModDepth.setTimeConstant(10e-3f);
    smoothVCFBendDepth.setTimeConstant(10e-3f);
    hpFilter.init(44100.0);
    vca.init(44100.0);
    chorus.init(44100.0);
}

void HeraSynthesiser::setCurrentPlaybackSampleRate(double newRate)
{
    MPESynthesiser::setCurrentPlaybackSampleRate(newRate);
    arp.setSampleRate(newRate);
    lfo.setSampleRate(newRate);
    smoothPitchModDepth.setSampleRate(newRate);
    smoothCutoff.setSampleRate(newRate);
    smoothResonance.setSampleRate(newRate);
    smoothVCFEnvModDepth.setSampleRate(newRate);
    smoothVCFLFOModDepth.setSampleRate(newRate);
    smoothVCFKeyboardModDepth.setSampleRate(newRate);
    smoothVCFBendDepth.setSampleRate(newRate);
    hpFilter.classInit(newRate);
    hpFilter.instanceConstants(newRate);
    hpFilter.instanceClear();
    vca.classInit(newRate);
    vca.instanceConstants(newRate);
    vca.instanceClear();
    chorus.classInit(newRate);
    chorus.instanceConstants(newRate);
    chorus.instanceClear();
}

void HeraSynthesiser::reserveBlockSize(int blockSize)
{
    lfoBuffer = juce::AudioBuffer<float>(1, blockSize);
    detuneBuffer = juce::AudioBuffer<float>(1, blockSize);

    dcoBuffer = juce::AudioBuffer<float>(1, blockSize);
    envelopeBuffer = juce::AudioBuffer<float>(1, blockSize);
    gateBuffer = juce::AudioBuffer<float>(1, blockSize);
    pwmModBuffer = juce::AudioBuffer<float>(1, blockSize);
    cutoffOctavesBuffer = juce::AudioBuffer<float>(1, blockSize);
    cutoffBuffer = juce::AudioBuffer<float>(1, blockSize);
    resonanceBuffer = juce::AudioBuffer<float>(1, blockSize);
    vcfEnvModBuffer = juce::AudioBuffer<float>(1, blockSize);
    vcfLFODetuneOctavesBuffer = juce::AudioBuffer<float>(1, blockSize);
    vcfKeyboardModBuffer = juce::AudioBuffer<float>(1, blockSize);
    vcfBendDepthBuffer = juce::AudioBuffer<float>(1, blockSize);
}

void HeraSynthesiser::setupParameters()
{
    if (parametersReady)
        return;

    int parameterDomains[kHeraNumParameters];

    // create parameters
    for (int p = 0; p < kHeraNumParameters; ++p) {
        juce::RangedAudioParameter *param = nullptr;
        HeraParameterDomain domain = (HeraParameterDomain)-1;

        param = createParameter(p, domain);
        parameterDomains[p] = domain;
        jassert(param);
        jassert((int)domain != -1);

        parameters[p].setOwned(param);
    }

    // set up parameter listeners
    for (int p = 0; p < kHeraNumParameters; ++p) {
        juce::RangedAudioParameter *param = parameters[p];
        int domain = parameterDomains[p];

        switch (domain) {
        case kHeraParameterIsForSynth:
            param->addListener(this);
            break;
        case kHeraParameterIsForVoice:
            for (int i = 0; i < kHeraMaxVoices; ++i) {
                HeraVoice *voice = static_cast<HeraVoice *>(getVoice(i));
                param->addListener(voice);
            }
            break;
        default:
            jassertfalse;
        }
    }

    parametersReady = true;
}

juce::AudioProcessorValueTreeState::ParameterLayout HeraSynthesiser::makeParameterLayout()
{
    setupParameters();

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> ownedParameters;
    ownedParameters.resize(kHeraNumParameters);
    for (int p = 0; p < kHeraNumParameters; ++p) {
        jassert(parameters[p].willDeleteObject());
        juce::RangedAudioParameter *param = parameters[p].get();
        ownedParameters[p].reset(param);
        parameters[p].setNonOwned(param);
    }

    return juce::AudioProcessorValueTreeState::ParameterLayout(ownedParameters.begin(), ownedParameters.end());
}

juce::RangedAudioParameter *HeraSynthesiser::createParameter(int index, HeraParameterDomain &domain)
{
    juce::RangedAudioParameter *param = nullptr;

    switch (index) {
    case kHeraParamVCA:
        param = new juce::AudioParameterFloat("VCADepth", "VCA depth", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.5f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCAType:
        param = new juce::AudioParameterChoice("VCAType", "VCA type", juce::StringArray{"Envelope", "Gate"}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamPWMDepth:
        param = new juce::AudioParameterFloat("DCOPWMDepth", "DCO PWM depth", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.5f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamPWMMod:
        param = new juce::AudioParameterChoice("DCOPWMMod", "DCO PWM modulator", juce::StringArray{"Manual", "LFO", "Envelope"}, 0);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamSawLevel:
        param = new juce::AudioParameterFloat("DCOSawLevel", "DCO saw level", juce::NormalisableRange<float>{0.0f, 1.0f}, 1.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamPulseLevel:
        param = new juce::AudioParameterFloat("DCOPulseLevel", "DCO pulse level", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamSubLevel:
        param = new juce::AudioParameterFloat("DCOSubLevel", "DCO sub level", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamNoiseLevel:
        param = new juce::AudioParameterFloat("DCONoiseLevel", "DCO noise level", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamPitchRange:
        param = new juce::AudioParameterChoice("DCORange", "DCO range", juce::StringArray{"16'", "8'", "4'"}, 1);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamPitchModDepth:
        param = new juce::AudioParameterFloat("DCOPitchModDepth", "DCO pitch mod depth", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCFCutoff:
        param = new juce::AudioParameterFloat("VCFCutoff", "VCF cutoff", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.5f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCFResonance:
        param = new juce::AudioParameterFloat("VCFResonance", "VCF resonance", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCFEnvelopeModDepth:
        param = new juce::AudioParameterFloat("VCFEnv", "VCF envelope mod depth", juce::NormalisableRange<float>{-1.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCFLFOModDepth:
        param = new juce::AudioParameterFloat("VCFLFO", "VCF LFO mod depth", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCFKeyboardModDepth:
        param = new juce::AudioParameterFloat("VCFKey", "VCF keyboard mod depth", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamVCFBendDepth:
        param = new juce::AudioParameterFloat("VCFBendDepth", "VCF bend depth", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamAttack:
        param = new juce::AudioParameterFloat("ENVAttack", "Envelope attack", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamDecay:
        param = new juce::AudioParameterFloat("ENVDecay", "Envelope decay", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamSustain:
        param = new juce::AudioParameterFloat("ENVSustain", "Envelope sustain", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamRelease:
        param = new juce::AudioParameterFloat("ENVRelease", "Envelope release", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForVoice;
        break;
    case kHeraParamLFOTriggerMode:
        param = new juce::AudioParameterChoice("LFOTrigMode", "LFO trigger mode", juce::StringArray{"Manual", "Auto"}, 1);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamLFORate:
        param = new juce::AudioParameterFloat("LFORate", "LFO rate", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamLFODelay:
        param = new juce::AudioParameterFloat("LFODelay", "LFO delay", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamHPF:
        param = new juce::AudioParameterFloat("HPF", "HPF", juce::NormalisableRange<float>{0.0f, 1.0f}, 0.0f);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamChorusI:
        param = new juce::AudioParameterBool("ChorusI", "Chorus I", false);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamChorusII:
        param = new juce::AudioParameterBool("ChorusII", "Chorus II", false);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamArpEnable:
        param = new juce::AudioParameterBool("ARPEnable", "Arpeggiator enable", false);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamArpMode:
        param = new juce::AudioParameterChoice("ARPMode", "Arpeggiator mode", {"Up", "Up/Down", "Down"}, 0);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamArpStep:
        param = new juce::AudioParameterChoice("ARPStep", "Arpeggiator step", {"1/4", "1/8", "1/16", "1/4T", "1/8T", "1/16T"}, 1);
        domain = kHeraParameterIsForSynth;
        break;
    case kHeraParamArpRange:
        param = new juce::AudioParameterInt("ARPRange", "Arpeggiator range", 1, 3, 1);
        domain = kHeraParameterIsForSynth;
        break;
    }

    return param;
}

const juce::OwnedArray<juce::RangedAudioParameter> &HeraSynthesiser::getReferenceParameters()
{
    static const auto parameters = []() {
        juce::OwnedArray<juce::RangedAudioParameter> list;
        list.ensureStorageAllocated(kHeraNumParameters);
        for (int p = 0; p < kHeraNumParameters; ++p) {
            HeraParameterDomain domain;
            list.add(HeraSynthesiser::createParameter(p, domain));
        }
        return list;
    }();
    return parameters;
}

void HeraSynthesiser::softClip(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    const LerpTable &softClip = curveSoftClipTanh3;
    //const LerpTable &softClip = curveSoftClipCubic;

    int numChannels = buffer.getNumChannels();
    for (int c = 0; c < numChannels; ++c) {
        float *channel = buffer.getWritePointer(c);
        for (int i = 0; i < numSamples; ++i)
            channel[startSample + i] = softClip(channel[startSample + i]);
    }
}

///
void HeraSynthesiser::renderNextBlockUsingArpeggiator(juce::AudioBuffer<float> &outputAudio, const juce::MidiBuffer &inputMidi, int startSample, int numSamples)
{
    renderNextBlock(outputAudio, arp.processBlock(inputMidi, numSamples), startSample, numSamples);
}

void HeraSynthesiser::renderNextSubBlock(juce::AudioBuffer<float> &outputAudio, int startSample, int numSamples)
{
    juce::AudioBuffer<float> monoAudio(outputAudio.getArrayOfWritePointers(), 1, outputAudio.getNumSamples());

    lfo.processBlock(lfoBuffer.getWritePointer(0), numSamples);

    processDetune(numSamples);
    processCutoffAndResonance(numSamples);

    MPESynthesiser::renderNextSubBlock(monoAudio, startSample, numSamples);

    compute(hpFilter, monoAudio, startSample, monoAudio, startSample, numSamples);
    compute(vca, monoAudio, startSample, monoAudio, startSample, numSamples);

    softClip(monoAudio, startSample, numSamples);

    compute(chorus, monoAudio, startSample, outputAudio, startSample, numSamples);
}

void HeraSynthesiser::processDetune(int numSamples)
{
    const float *lfoIn = lfoBuffer.getReadPointer(0);
    float *detuneOut = detuneBuffer.getWritePointer(0);
    float detuneFactor = pitchFactor;
    for (int i = 0; i < numSamples; ++i)
        detuneOut[i] = detuneFactor * std::exp2(lfoIn[i] * 0.25f * smoothPitchModDepth.getNextValue());
}

void HeraSynthesiser::processCutoffAndResonance(int numSamples)
{
    float *cutoffOctavesOut = cutoffOctavesBuffer.getWritePointer(0);
    float *resonanceOut = resonanceBuffer.getWritePointer(0);
    float *vcfEnvModOut = vcfEnvModBuffer.getWritePointer(0);
    float *vcfLFODetuneOctavesOut = vcfLFODetuneOctavesBuffer.getWritePointer(0);
    float *vcfKeyboardModOut = vcfKeyboardModBuffer.getWritePointer(0);
    float *vcfBendDepthOut = vcfBendDepthBuffer.getWritePointer(0);
    const float *lfoIn = lfoBuffer.getReadPointer(0);

    for (int i = 0; i < numSamples; ++i) {
        float cutoff = smoothCutoff.getNextValue();
        float resonance = smoothResonance.getNextValue();

        float cutoffDetuneOctaves = cutoff * (200.0f / 12.0f);
        float resonanceDetuneOctaves = resonance * 0.5f;

        cutoffOctavesOut[i] = cutoffDetuneOctaves + resonanceDetuneOctaves;
        resonanceOut[i] = resonance;
        vcfEnvModOut[i] = smoothVCFEnvModDepth.getNextValue();
        vcfLFODetuneOctavesOut[i] = smoothVCFLFOModDepth.getNextValue() * lfoIn[i] * 3.0f;
        vcfKeyboardModOut[i] = smoothVCFKeyboardModDepth.getNextValue();
        vcfBendDepthOut[i] = smoothVCFBendDepth.getNextValue();
    }
}

void HeraSynthesiser::parameterValueChanged(int parameterIndex, float newValue)
{
    newValue = parameters[parameterIndex]->convertFrom0to1(newValue);

    switch (parameterIndex) {
    case kHeraParamPitchRange:
    {
        constexpr float factors[] = { 0.5f, 1.0f, 2.0f };
        pitchFactor = factors[juce::jlimit(0, 2, (int)newValue)];
        break;
    }
    case kHeraParamPitchModDepth:
        smoothPitchModDepth.setTargetValue(newValue);
        break;
    case kHeraParamVCFCutoff:
        smoothCutoff.setTargetValue(newValue);
        break;
    case kHeraParamVCFResonance:
        smoothResonance.setTargetValue(newValue);
        break;
    case kHeraParamVCFEnvelopeModDepth:
        smoothVCFEnvModDepth.setTargetValue(newValue);
        break;
    case kHeraParamVCFLFOModDepth:
        smoothVCFLFOModDepth.setTargetValue(newValue);
        break;
    case kHeraParamVCFKeyboardModDepth:
        smoothVCFKeyboardModDepth.setTargetValue(newValue);
        break;
    case kHeraParamVCFBendDepth:
        smoothVCFBendDepth.setTargetValue(newValue);
        break;
    case kHeraParamVCA:
        vca.setAmount(newValue);
        break;
    case kHeraParamVCAType:
        vcaType = (int)newValue;
        break;
    case kHeraParamLFOTriggerMode: {
        int newLFOMode = (int)newValue;
        if (lfoMode != newLFOMode) {
            lfo.shutdown();
            lfoMode = newLFOMode;
        }
        break;
    }
    case kHeraParamLFORate:
        lfo.setFrequency(curveFromLfoRateSliderToFreq(newValue));
        break;
    case kHeraParamLFODelay:
        lfo.setDelayDuration(curveFromLfoDelaySliderToDelay(newValue));
        lfo.setAttackDuration(curveFromLfoDelaySliderToAttack(newValue));
        break;
    case kHeraParamHPF:
        hpFilter.setAmount(newValue);
        break;
    case kHeraParamChorusI:
        chorus.setChorusI(newValue);
        break;
    case kHeraParamChorusII:
        chorus.setChorusII(newValue);
        break;
    case kHeraParamArpEnable:
        arp.setEnabled(newValue > 0.5);
        break;
    case kHeraParamArpMode:
        arp.setMode((int)newValue);
        break;
    case kHeraParamArpStep:
        arp.setStep((int)newValue);
        break;
    case kHeraParamArpRange:
        arp.setRange((int)newValue);
        break;
    }
}

void HeraSynthesiser::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    juce::ignoreUnused(parameterIndex);
    juce::ignoreUnused(gestureIsStarting);
}

bool HeraSynthesiser::hasUnreleasedVoices() const
{
    int numVoices = getNumVoices();
    bool haveVoicesOn = false;
    for (int i = 0; i < numVoices && !haveVoicesOn; ++i) {
        HeraVoice *voice = static_cast<HeraVoice *>(getVoice(i));
        haveVoicesOn = !voice->getCurrentEnvelope().isReleased();
    }
    return haveVoicesOn;
}

HeraVoice::HeraVoice(HeraSynthesiser *synthesiser)
    : synthesiser(synthesiser)
{
    normalEnvelope.setSampleRate(44100.0);
    gateEnvelope.setSampleRate(44100.0);
    dco.init(44100.0);

    gateEnvelope.setAttack(0.00247f);
    gateEnvelope.setDecay(0.0057f);
    gateEnvelope.setSustain(0.98f);
    gateEnvelope.setRelease(0.0057f);

    smoothPWMDepth.setTimeConstant(10e-3f);
}

void HeraVoice::setCurrentSampleRate(double newRate)
{
    MPESynthesiserVoice::setCurrentSampleRate(newRate);
    normalEnvelope.setSampleRate(newRate);
    gateEnvelope.setSampleRate(newRate);
    dco.classInit(newRate);
    dco.instanceConstants(newRate);
    dco.instanceClear();
    vcf.setSampleRate(newRate);
    smoothPWMDepth.setSampleRate(newRate);
}

void HeraVoice::noteStarted()
{
    HeraSynthesiser &synthesiser = *this->synthesiser;

    vcaType = synthesiser.vcaType;

    if (synthesiser.lfoMode == kHeraLFOAuto) {
        if (!synthesiser.hasUnreleasedVoices())
            synthesiser.lfo.noteOn();
    }

    HeraEnvelope &envelope = getCurrentEnvelope();
    envelope.noteOn();

    dco.setFrequency(currentlyPlayingNote.getFrequencyInHertz());
    flushSmoothValues(dco);

    smoothPWMDepth.setCurrentAndTargetValue(smoothPWMDepth.getTargetValue());
}

void HeraVoice::noteStopped(bool allowTailOff)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    HeraEnvelope &envelope = getCurrentEnvelope();

    if (allowTailOff)
        envelope.noteOff();
    else
        envelope.shutdown();

    if (synthesiser.lfoMode == kHeraLFOAuto) {
        if (!synthesiser.hasUnreleasedVoices())
            synthesiser.lfo.noteOff();
    }
}

void HeraVoice::notePressureChanged()
{
}

void HeraVoice::notePitchbendChanged()
{
    dco.setFrequency(currentlyPlayingNote.getFrequencyInHertz());
}

void HeraVoice::noteTimbreChanged()
{
}

void HeraVoice::noteKeyStateChanged()
{
}

void HeraVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    processEnvelope(numSamples);
    processPWM(numSamples);
    processDCO(numSamples);
    processVCF(numSamples);
    processMix(outputBuffer, startSample, numSamples);

    HeraEnvelope &envelope = getCurrentEnvelope();
    if (!envelope.isActive()) {
        envelope.reset();
        gateEnvelope.reset();
        dco.instanceClear();
        vcf.reset();
        clearCurrentNote();
    }
}

void HeraVoice::processEnvelope(int numSamples)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    float *envelopeOut = synthesiser.envelopeBuffer.getWritePointer(0);
    normalEnvelope.processNextBlock(envelopeOut, 0, numSamples);
    if (vcaType != kHeraVCATypeEnvelope) {
        float *gateOut = synthesiser.gateBuffer.getWritePointer(0);
        gateEnvelope.processNextBlock(gateOut, 0, numSamples);
    }
}

void HeraVoice::processPWM(int numSamples)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    juce::AudioBuffer<float> &pwmModBuffer = synthesiser.pwmModBuffer;
    const float *lfoIn = synthesiser.lfoBuffer.getReadPointer(0);
    const float *envelopeIn = synthesiser.envelopeBuffer.getReadPointer(0);
    float *pwmModOut = pwmModBuffer.getWritePointer(0);
    switch (pwmMod) {
    default:
        for (int i = 0; i < numSamples; ++i)
            pwmModOut[i] = smoothPWMDepth.getNextValue();
        break;
    case kHeraPWMLFO:
        for (int i = 0; i < numSamples; ++i)
            pwmModOut[i] = smoothPWMDepth.getNextValue() * (lfoIn[i] * 0.5f + 0.5f);
        break;
    case kHeraPWMEnvelope:
        for (int i = 0; i < numSamples; ++i)
            pwmModOut[i] = smoothPWMDepth.getNextValue() * envelopeIn[i];
        break;
    }
}

void HeraVoice::processDCO(int numSamples)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    const float *detuneOut = synthesiser.detuneBuffer.getReadPointer(0);
    const float *pwmModOut = synthesiser.pwmModBuffer.getReadPointer(0);
    float *dcoOut = synthesiser.dcoBuffer.getWritePointer(0);
    compute(dco, { detuneOut, pwmModOut }, { dcoOut }, numSamples);
}

void HeraVoice::processVCF(int numSamples)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    float *dcoOut = synthesiser.dcoBuffer.getWritePointer(0);
    const float *modEnvelopeIn = synthesiser.envelopeBuffer.getReadPointer(0);
    const float *ampEnvelopeIn = (vcaType == kHeraVCATypeEnvelope) ?
        synthesiser.envelopeBuffer.getReadPointer(0) :
        synthesiser.gateBuffer.getReadPointer(0);
    const float *cutoffOctaves = synthesiser.cutoffOctavesBuffer.getReadPointer(0);
    float *cutoff = synthesiser.cutoffBuffer.getWritePointer(0);
    const float *resonance = synthesiser.resonanceBuffer.getReadPointer(0);
    const float *vcfEnvMod = synthesiser.vcfEnvModBuffer.getReadPointer(0);
    const float *vcfLFODetuneOctaves = synthesiser.vcfLFODetuneOctavesBuffer.getReadPointer(0);
    const float *vcfKeyboardMod = synthesiser.vcfKeyboardModBuffer.getReadPointer(0);
    const float *vcfBendDepth = synthesiser.vcfBendDepthBuffer.getReadPointer(0);

    int filterNote = currentlyPlayingNote.initialNote;
    float filterNoteFactor = (float)(filterNote - 60)  * (1.0f / 12);

    // don't have the direct pitchbend value in MPESynthesiser
    // apply ratio from dco bend -> filter bend, hoping it's correct
    float pitchbendSemitones = (float)currentlyPlayingNote.totalPitchbendInSemitones;
    float pitchbendFactor = pitchbendSemitones * (48.0f / (12.0f * 7.0f));

    for (int i = 0; i < numSamples; ++i) {
        float envDetuneOctaves = vcfEnvMod[i] * modEnvelopeIn[i] * 12;
        float lfoDetuneOctaves = vcfLFODetuneOctaves[i] * ampEnvelopeIn[i];
        float keyboardDetuneOctaves = vcfKeyboardMod[i] * filterNoteFactor;
        float filterBendOctaves = vcfBendDepth[i] * pitchbendFactor;
        cutoff[i] = 7.8f * std::exp2(cutoffOctaves[i] + envDetuneOctaves + lfoDetuneOctaves + keyboardDetuneOctaves + filterBendOctaves);
    }

    vcf.processNextBlock(dcoOut, cutoff, resonance, numSamples);
}

void HeraVoice::processMix(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    float *mixOut = outputBuffer.getWritePointer(0);
    const float *dcoIn = synthesiser.dcoBuffer.getReadPointer(0);
    const float *envelopeIn = (vcaType == kHeraVCATypeEnvelope) ?
        synthesiser.envelopeBuffer.getReadPointer(0) :
        synthesiser.gateBuffer.getReadPointer(0);

    // velocity to volume according to MMA recommendation
    float velocity = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    float noteVolume = velocity * velocity;
    // compensate the attenuation at center velocity
    noteVolume *= 2.0f;

    for (int i = 0; i < numSamples; ++i)
        mixOut[startSample + i] += dcoIn[i] * envelopeIn[i] * noteVolume;
}

void HeraVoice::parameterValueChanged(int parameterIndex, float newValue)
{
    HeraSynthesiser &synthesiser = *this->synthesiser;
    newValue = synthesiser.parameters[parameterIndex]->convertFrom0to1(newValue);

    switch (parameterIndex) {
    case kHeraParamPWMDepth:
        smoothPWMDepth.setTargetValue(newValue);
        break;
    case kHeraParamPWMMod:
        pwmMod = (int)newValue;
        break;
    case kHeraParamSawLevel:
        dco.setSawLevel(newValue);
        break;
    case kHeraParamPulseLevel:
        dco.setPulseLevel(newValue);
        break;
    case kHeraParamSubLevel:
        dco.setSubLevel(newValue);
        break;
    case kHeraParamNoiseLevel:
        dco.setNoiseLevel(newValue);
        break;
    case kHeraParamAttack:
        normalEnvelope.setAttack(newValue);
        break;
    case kHeraParamDecay:
        normalEnvelope.setDecay(newValue);
        break;
    case kHeraParamSustain:
        normalEnvelope.setSustain(newValue);
        break;
    case kHeraParamRelease:
        normalEnvelope.setRelease(newValue);
        break;
    }
}

void HeraVoice::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    juce::ignoreUnused(parameterIndex);
    juce::ignoreUnused(gestureIsStarting);
}
