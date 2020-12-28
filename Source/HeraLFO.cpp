// SPDX-License-Identifier: GPL-3.0-or-later

#include "HeraLFO.h"
#include "LerpTable.h"
#include <cmath>

const LerpTable &getSineTable()
{
    static const LerpTable sine(
        [](double x) -> double { return std::sin(2.0 * M_PI * x); },
        0.0, 1.0, 128);
    return sine;
}

HeraLFO::HeraLFO()
{
    getSineTable();

    smoothFrequency_.setTimeConstant(10e-3);
}

void HeraLFO::setSampleRate(double newRate)
{
    sampleRate_ = newRate;
    smoothFrequency_.setSampleRate(newRate);
    reset();
}

void HeraLFO::processBlock(float *output, int numFrames)
{
    float phase = currentPhase_;
    float value = currentValue_;
    float samplePeriod = 1.0f / sampleRate_;

    switch (type_) {
    default:
        for (int i = 0; i < numFrames; ++i) {
            output[i] = value = 0.0f;
            phase += smoothFrequency_.getNextValue() * samplePeriod;
            //cycle[i] = phase >= 1.0f;
            phase -= (int)phase;
        }
        return;
    case Triangle:
        for (int i = 0; i < numFrames; ++i) {
            value = -4 * phase + 2;
            value = (phase < 0.25f) ? (4 * phase) : value;
            value = (phase > 0.75f) ? (4 * phase - 4) : value;
            output[i] = value;
            phase += smoothFrequency_.getNextValue() * samplePeriod;
            //cycle[i] = phase >= 1.0f;
            phase -= (int)phase;
        }
        break;
    case Sine:
    {
        const LerpTable &sine = getSineTable();
        for (int i = 0; i < numFrames; ++i) {
            output[i] = value = sine(phase);
            phase += smoothFrequency_.getNextValue() * samplePeriod;
            //cycle[i] = phase >= 1.0f;
            phase -= (int)phase;
        }
        break;
    }
    case Square:
        for (int i = 0; i < numFrames; ++i) {
            output[i] = value = (phase < 0.5f) ? -1.0f : 1.0f;
            phase += smoothFrequency_.getNextValue() * samplePeriod;
            //cycle[i] = phase >= 1.0f;
            phase -= (int)phase;
        }
        break;
    case Random:
    {
        juce::Random &prng = prng_;
        for (int i = 0; i < numFrames; ++i) {
            output[i] = value;
            phase += smoothFrequency_.getNextValue() * samplePeriod;
            int cyc = phase >= 1.0f;
            //cycle[i] = cyc;
            phase -= (int)phase;
            if (cyc)
                value = prng.nextFloat() * 2.0f - 1.0f;
        }
        break;
    }
    case Noise:
    {
        juce::Random &prng = prng_;
        for (int i = 0; i < numFrames; ++i) {
            output[i] = value = prng.nextFloat() * 2.0f - 1.0f;
            phase += smoothFrequency_.getNextValue() * samplePeriod;
            //cycle[i] = phase >= 1.0f;
            phase -= (int)phase;
        }
        break;
    }
    }

    currentPhase_ = phase;
    currentValue_ = value;
}
