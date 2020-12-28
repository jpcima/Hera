// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

struct EnvelopeSegment {
    enum Type { Delay, Attack, Decay, Shutdown };
    int type = Delay;
    float tco = 0;
    float target = 0;
    bool sustained = false;
};

class AbstractEnvelope {
public:
    explicit AbstractEnvelope(std::vector<EnvelopeSegment> segments);
    void setSampleRate(float sampleRate);
    void setDuration(int segmentIndex, float duration);
    void setTarget(int segmentIndex, float target);
    void processNextBlock(float *output, int startSample, int numSamples);
    void applyEnvelopeToBuffer(juce::AudioBuffer<float> &buffer, int startSample, int numSamples);
    void reset();
    void noteOn();
    void noteOff();
    void shutdown();
    bool isActive() const { return currentPhase_ != -1; }
    bool isReleased() const;

private:
    void recalculateSegment(int segmentIndex);
    void resetSegment(int segmentIndex);

private:
    struct SegmentData {
        float duration;
        union {
            struct { float remaining; } delay;
            struct { float coeff; float offset; } ad;
            struct { float rate; } shutdown;
        } u;
    };
    float sampleRate_ = 0;
    int currentPhase_ = -1;
    float currentValue_ = 0.0;
    std::vector<EnvelopeSegment> segments_;
    std::vector<SegmentData> data_;
};

class HeraEnvelope {
public:
    HeraEnvelope();
    void setSampleRate(float sampleRate);
    void setAttack(float value);
    void setDecay(float value);
    void setSustain(float value);
    void setRelease(float value);
    void processNextBlock(float *output, int startSample, int numSamples) { envelope.processNextBlock(output, startSample, numSamples); }
    void applyEnvelopeToBuffer(juce::AudioBuffer<float> &buffer, int startSample, int numSamples) { envelope.applyEnvelopeToBuffer(buffer, startSample, numSamples); }
    void reset() { envelope.reset(); }
    void noteOn() { envelope.noteOn(); }
    void noteOff() { envelope.noteOff(); }
    void shutdown() { envelope.shutdown(); }
    bool isActive() const { return envelope.isActive(); }
    bool isReleased() const { return envelope.isReleased(); }

private:
    void recalculateValues();

private:
    float attackDuration = 0;
    float decayDuration = 0;
    float sustainLevel = 0;
    float releaseDuration = 0;
    AbstractEnvelope envelope;
};
