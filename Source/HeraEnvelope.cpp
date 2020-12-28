// SPDX-License-Identifier: GPL-3.0-or-later

#include "HeraEnvelope.h"
#include "LerpTable.h"
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cstring>
#include <cmath>

AbstractEnvelope::AbstractEnvelope(std::vector<EnvelopeSegment> segments)
{
    int numSegments = (int)segments.size();
    jassert(numSegments >= 2 &&
            segments[numSegments - 2].type == EnvelopeSegment::Decay &&
            segments[numSegments - 1].type == EnvelopeSegment::Shutdown);

    segments_ = std::move(segments);
    data_.resize(numSegments);
    for (int i = 0; i < numSegments; ++i) {
        SegmentData &data = data_[i];
        data.duration = 0;
    }
    setSampleRate(44100);
}

void AbstractEnvelope::setSampleRate(float sampleRate)
{
    sampleRate_ = sampleRate;
    int numSegments = (int)segments_.size();
    for (int i = 0; i < numSegments; ++i)
        recalculateSegment(i);
}

void AbstractEnvelope::setDuration(int segmentIndex, float duration)
{
    int numSegments = (int)segments_.size();
    jassert(segmentIndex >= 0 && segmentIndex < numSegments);
    juce::ignoreUnused(numSegments);

    SegmentData &data = data_[segmentIndex];
    data.duration = duration;
    recalculateSegment(segmentIndex);
}

void AbstractEnvelope::setTarget(int segmentIndex, float target)
{
    int numSegments = (int)segments_.size();
    jassert(segmentIndex >= 0 && segmentIndex < numSegments);
    juce::ignoreUnused(numSegments);

    EnvelopeSegment &segment = segments_[segmentIndex];
    segment.target = target;
    recalculateSegment(segmentIndex);
}

void AbstractEnvelope::processNextBlock(float *output, int startSample, int numSamples)
{
    int phase = currentPhase_;
    float value = currentValue_;
    int numSegments = (int)segments_.size();

    while (numSamples > 0) {
        EnvelopeSegment *segment = nullptr;
        SegmentData *data = nullptr;
        int sampleIndex = 0;

        int segmentType = -1;
        if (phase != -1) {
            segment = &segments_[phase];
            data = &data_[phase];
            segmentType = segment->type;
        }

        switch (segmentType) {
        case EnvelopeSegment::Delay: {
            float ts = 1.0f / sampleRate_;
            float rem = data->u.delay.remaining;
            for (; sampleIndex < numSamples; ++sampleIndex) {
                rem -= ts;
                if (rem < 0.0f) {
                    phase = (phase + 1 < numSegments) ? (phase + 1) : -1;
                    break;
                }
                value = 0.0f;
                output[startSample + sampleIndex] = 0.0f;
            }
            data->u.delay.remaining = rem;
            break;
        }
        case EnvelopeSegment::Attack: {
            bool sustained = segment->sustained;
            float target = segment->target;
            auto ad = data->u.ad;
            for (; sampleIndex < numSamples; ++sampleIndex) {
                float next = value * ad.coeff + ad.offset;
                if (next > target && !sustained) {
                    phase = (phase + 1 < numSegments) ? (phase + 1) : -1;
                    break;
                }
                value = juce::jmin(target, next);
                output[startSample + sampleIndex] = value;
            }
            break;
        }
        case EnvelopeSegment::Decay: {
            bool sustained = segment->sustained;
            float target = segment->target;
            auto ad = data->u.ad;
            for (; sampleIndex < numSamples; ++sampleIndex) {
                float next = value * ad.coeff + ad.offset;
                if ((next <= target && !sustained) || next < 0.02f) {
                    phase = (phase + 1 < numSegments) ? (phase + 1) : -1;
                    break;
                }
                value = juce::jmax(target, next);
                output[startSample + sampleIndex] = value;
            }
            break;
        }
        case EnvelopeSegment::Shutdown: {
            float rate = data->u.shutdown.rate;
            for (; sampleIndex < numSamples; ++sampleIndex) {
                float next = value - rate;
                if (next <= 0.0f) {
                    phase = (phase + 1 < numSegments) ? (phase + 1) : -1;
                    break;
                }
                value = juce::jmax(0.0f, next);
                output[startSample + sampleIndex] = value;
            }
            break;
        }
        default:
            std::memset(output + startSample, 0, numSamples * sizeof(float));
            value = 0;
            numSamples = 0;
            sampleIndex = numSamples;
            break;
        }

        startSample += sampleIndex;
        numSamples -= sampleIndex;
    }

    currentPhase_ = phase;
    currentValue_ = value;
}

void AbstractEnvelope::applyEnvelopeToBuffer(juce::AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    constexpr int maxSubdiv = 512;
    int numChannels = buffer.getNumChannels();

    while (numSamples > 0) {
        int numSubdiv = juce::jmin(maxSubdiv, numSamples);

        float envelopeOut[maxSubdiv];
        processNextBlock(envelopeOut, 0, numSubdiv);

        for (int c = 0; c < numChannels; ++c) {
            float *channel = buffer.getWritePointer(c);
            for (int i = 0; i < numSubdiv; ++i)
                channel[startSample + i] *= envelopeOut[i];
        }

        startSample += numSubdiv;
        numSamples -= numSubdiv;
    }
}

void AbstractEnvelope::reset()
{
    currentPhase_ = -1;
    currentValue_ = 0.0;
    int numSegments = (int)segments_.size();
    for (int i = 0; i < numSegments; ++i)
        resetSegment(i);
}

void AbstractEnvelope::noteOn()
{
    if (false)
        reset();
    else {
        int numSegments = (int)segments_.size();
        for (int i = 0; i < numSegments; ++i)
            resetSegment(i);
    }
    currentPhase_ = 0;
}

void AbstractEnvelope::noteOff()
{
    int numSegments = (int)segments_.size();
    if (currentPhase_ != -1)
        currentPhase_ = juce::jmax(currentPhase_, numSegments - 2);
}

bool AbstractEnvelope::isReleased() const
{
    int numSegments = (int)segments_.size();
    return currentPhase_ == -1 || currentPhase_ >= numSegments - 2;
}

void AbstractEnvelope::shutdown()
{
    int numSegments = (int)segments_.size();
    if (currentPhase_ != -1)
        currentPhase_ = juce::jmax(currentPhase_, numSegments - 1);
}

void AbstractEnvelope::recalculateSegment(int segmentIndex)
{
    int numSegments = (int)segments_.size();
    jassert(segmentIndex >= 0 && segmentIndex < numSegments);
    juce::ignoreUnused(numSegments);

    EnvelopeSegment &segment = segments_[segmentIndex];
    SegmentData &data = data_[segmentIndex];
    switch (segment.type) {
    case EnvelopeSegment::Delay:
        data.u.delay.remaining = data.duration;
        break;
    case EnvelopeSegment::Attack: {
        float tco = juce::jmax(0.001f, segment.tco);
        float samples = sampleRate_ * juce::jmax(0.001f, data.duration);
        data.u.ad.coeff = std::exp(-std::log((1.0f + tco) / tco) / samples);
        data.u.ad.offset = (1.0f + tco) * (1.0f - data.u.ad.coeff);
        break;
    }
    case EnvelopeSegment::Decay: {
        float tco = juce::jmax(0.001f, segment.tco);
        float samples = sampleRate_ * juce::jmax(0.001f, data.duration);
        data.u.ad.coeff = std::exp(-std::log((1.0f + tco) / tco) / samples);
        data.u.ad.offset = (segment.target - tco) * (1.0f - data.u.ad.coeff);
        break;
    }
    case EnvelopeSegment::Shutdown: {
        float tco = juce::jmax(0.001f, segment.tco);
        data.u.shutdown.rate = 1.0f / (tco * sampleRate_);
        break;
    }
    }
}

void AbstractEnvelope::resetSegment(int segmentIndex)
{
    int numSegments = (int)segments_.size();
    jassert(segmentIndex >= 0 && segmentIndex < numSegments);
    juce::ignoreUnused(numSegments);

    EnvelopeSegment &segment = segments_[segmentIndex];
    SegmentData &data = data_[segmentIndex];
    switch (segment.type) {
    case EnvelopeSegment::Delay:
        data.u.delay.remaining = data.duration;
        break;
    }
}

///
static const std::array<EnvelopeSegment, 4> kHeraSegments {{
    {EnvelopeSegment::Attack, 0.632f, 1.0f, false},
    {EnvelopeSegment::Decay, 0.025f, 0.0f, true},
    {EnvelopeSegment::Decay, 0.025f, 0.0f, false},
    {EnvelopeSegment::Shutdown, 0.001f},
}};

static const LerpTable &getAttackCurve()
{
    static const LerpTable curve(
        [](double x) -> double { return 0.0099 * std::exp(5.8 * x); },
        0.0, 1.0, 128);
    return curve;
}

static const LerpTable &getDecayReleaseCurve()
{
    static const LerpTable curve(
        [](double x) -> double { return 0.0495 * std::exp(6.0 * x); },
        0.0, 1.0, 128);
    return curve;
}

///
HeraEnvelope::HeraEnvelope()
    : envelope(std::vector<EnvelopeSegment>(kHeraSegments.begin(), kHeraSegments.end()))
{
    recalculateValues();
}

void HeraEnvelope::setSampleRate(float sampleRate)
{
    envelope.setSampleRate(sampleRate);
}

void HeraEnvelope::setAttack(float value)
{
    attackDuration = getAttackCurve()(value);
    recalculateValues();
}

void HeraEnvelope::setDecay(float value)
{
    decayDuration = getDecayReleaseCurve()(value);
    recalculateValues();
}

void HeraEnvelope::setSustain(float value)
{
    sustainLevel = value;
    recalculateValues();
}

void HeraEnvelope::setRelease(float value)
{
    releaseDuration = getDecayReleaseCurve()(value);
    recalculateValues();
}

void HeraEnvelope::recalculateValues()
{
    float decayTarget = juce::jmax(0.02f, sustainLevel);
    envelope.setDuration(0, attackDuration);
    envelope.setTarget(1, decayTarget);
    envelope.setDuration(1, decayDuration);
    envelope.setDuration(2, (decayTarget <= 0.02f) ? 0.01f : releaseDuration);
}
