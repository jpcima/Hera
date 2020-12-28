// SPDX-License-Identifier: ISC

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <initializer_list>

template <class FaustDSP>
static void compute(
    FaustDSP &dsp,
    const juce::AudioBuffer<float> &input, int inputStartSample,
    juce::AudioBuffer<float> &output, int outputStartSample,
    int numSamples)
{
    const float *inPtr[FaustDSP::getNumInputs()];
    float *outPtr[FaustDSP::getNumOutputs()];
    for (int i = 0; i < FaustDSP::getNumInputs(); ++i)
        inPtr[i] = input.getReadPointer(i) + inputStartSample;
    for (int i = 0; i < FaustDSP::getNumOutputs(); ++i)
        outPtr[i] = output.getWritePointer(i) + outputStartSample;
    dsp.compute(numSamples, (float **)inPtr, outPtr);
}

template <class FaustDSP>
static void compute(
    FaustDSP &dsp,
    std::initializer_list<const float *> inputs,
    std::initializer_list<float *> outputs,
    int numSamples)
{
    jassert(inputs.size() == FaustDSP::getNumInputs());
    jassert(outputs.size() == FaustDSP::getNumOutputs());
    dsp.compute(numSamples, (float **)inputs.begin(), (float **)outputs.begin());
}

template <class FaustDSP>
static void flushSmoothValues(FaustDSP &dsp)
{
    float in[FaustDSP::getNumInputs()] = {};
    float out[FaustDSP::getNumOutputs()];

    float *inPtr[FaustDSP::getNumInputs()];
    float *outPtr[FaustDSP::getNumOutputs()];
    for (int i = 0; i < FaustDSP::getNumInputs(); ++i)
        inPtr[i] = &in[i];
    for (int i = 0; i < FaustDSP::getNumOutputs(); ++i)
        outPtr[i] = &out[i];

    dsp.setSmoothDisabled(1);
    dsp.compute(1, inPtr, outPtr);
    dsp.setSmoothDisabled(0);
}
