#pragma once
#include "HeraDefs.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <vf_core/vf_core.h>

class HeraArpeggiator {
public:
    HeraArpeggiator();
    void reset();
    bool hasActiveNotes() const;
    void setTempo(double newTempo);
    void setSampleRate(double newRate);
    void setEnabled(bool newEnabled);
    void setMode(int newMode);
    void setStep(int newStep);
    void setRange(int newRange);
    const juce::MidiBuffer &processBlock(const juce::MidiBuffer &inputMidi, int numSamples);

private:
    struct ArpegNote;
    struct ArpegChannel;
    typedef vf::List<ArpegNote> ArpegNoteList;
    void tickArpeggio(juce::MidiBuffer &outputMidi, int startSample, int numSamples);
    void sequenceNextNoteOn(juce::MidiBuffer &outputMidi, int startSample);
    void sequenceNextNoteOff(juce::MidiBuffer &outputMidi, int startSample);
    void advanceToNextKey(ArpegChannel &ac);
    void noteOn(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1, int note, int velocity);
    void noteOff(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1, int note, int velocity);
    void allNotesOff(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1);
    void allSoundOff(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1);
    static void insertNoteInOrder(ArpegNote &an, ArpegNoteList &list);
    void initializeNoteBuffer();
    void performReset(juce::MidiBuffer &outputMidi, int startSample);

    static bool keyIsInMidiRange(int key) { return key >= 0 && key < 128; }

private:
    // parameters
    bool enabled = false;
    double noteOnInterval = 1.0;
    int octaves = 1;
    int mode = kHeraArpModeUp;

    // settings from host
    double currentSampleRate = 44100.0;
    double beatsPerSecond = 0.5;

    struct ArpegNote : ArpegNoteList::Node {
        int channelFrom1 = 0;
        int key = 0;
        //
        bool active = false;
        int playingVelocity = 0;
    };

    struct ArpegChannel {
        int channelFrom1 = 0;
        //
        int initialKey = 0;
        int octave = 0;
        int direction = +1; // always -1 or +1
        bool hasPlayingKey = false;
        bool isNoteOnPlayingKey = false;
        int playingKeyNoOctave = 0;
        //
        ArpegNoteList activeNotes;
        ArpegNote allNotes[128];
        //
        bool active() const { return !activeNotes.empty(); }
        void noteOnPlayingKey(juce::MidiBuffer &outputMidi, int startSample);
        void noteOffPlayingKey(juce::MidiBuffer &outputMidi, int startSample);
    };

    // arpeggio status
    ArpegChannel arpChannel[16];
    double currentTickPosition = 0.0;

    // whether to reset, to happen at next block start
    bool resetPending = false;

    // temporary
    juce::MidiBuffer midiBuffer;
};
