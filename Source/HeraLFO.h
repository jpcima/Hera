// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "SmoothValue.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

class HeraLFO {
public:
    enum Type { Triangle, Sine, Square, Random, Noise, None = -1 };

    HeraLFO();
    void setSampleRate(double newRate);
    void processBlock(float *output, int numFrames);
    void setFrequency(float freq) { smoothFrequency_.setTargetValue(freq); }
    void setType(int type) { type_ = type; reset(); }
    void reset() { currentPhase_ = 0; currentValue_ = 0; }

private:
    int type_ = Triangle;
    float sampleRate_ = 44100;
    OnePoleSmoothValue smoothFrequency_;
    float currentPhase_ = 0;
    float currentValue_ = 0;
    juce::Random prng_;
};
