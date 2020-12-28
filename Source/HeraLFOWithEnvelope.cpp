// SPDX-License-Identifier: GPL-3.0-or-later

#include "HeraLFOWithEnvelope.h"
#include <cstring>

static const std::array<EnvelopeSegment, 4> kHeraLFOSegments {{
    {EnvelopeSegment::Delay},
    {EnvelopeSegment::Attack, 0.03f, 1.0f, true},
    {EnvelopeSegment::Decay, 0.025f, 0.0f, false},
    {EnvelopeSegment::Shutdown, 0.001f},
}};

HeraLFOWithEnvelope::HeraLFOWithEnvelope()
    : envelope(std::vector<EnvelopeSegment>(kHeraLFOSegments.begin(), kHeraLFOSegments.end()))
{
    envelope.setDuration(2, 0.1f);
}

void HeraLFOWithEnvelope::setSampleRate(double newRate)
{
    lfo.setSampleRate(newRate);
    envelope.setSampleRate(newRate);
}

void HeraLFOWithEnvelope::processBlock(float *output, int numFrames)
{
    if (!envelope.isActive()) {
        std::memset(output, 0, numFrames * sizeof(float));
        return;
    }

    lfo.processBlock(output, numFrames);

    juce::AudioBuffer<float> envelopeIn(&output, 1, numFrames);
    envelope.applyEnvelopeToBuffer(envelopeIn, 0, numFrames);
}

void HeraLFOWithEnvelope::noteOn()
{
    bool active = isActive();
    if (!active)
        lfo.reset();
    envelope.noteOn();
}

void HeraLFOWithEnvelope::setDelayDuration(float duration)
{
    envelope.setDuration(0, duration);
}

void HeraLFOWithEnvelope::setAttackDuration(float duration)
{
    envelope.setDuration(1, duration);
}
