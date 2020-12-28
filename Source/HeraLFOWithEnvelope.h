// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "HeraLFO.h"
#include "HeraEnvelope.h"

class HeraLFOWithEnvelope {
public:
    HeraLFOWithEnvelope();
    void setSampleRate(double newRate);
    void processBlock(float *output, int numFrames);
    void reset() { lfo.reset(); envelope.reset(); }
    void noteOn();
    void noteOff() { envelope.noteOff(); }
    void shutdown() { envelope.shutdown(); }
    void setFrequency(float freq) { lfo.setFrequency(freq); }
    void setType(int type) { lfo.setType(type); }
    void setDelayDuration(float duration);
    void setAttackDuration(float duration);
    bool isActive() const { return envelope.isActive(); }

private:
    HeraLFO lfo;
    AbstractEnvelope envelope;
};
