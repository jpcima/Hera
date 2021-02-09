#include "HeraArpeggiator.h"
#include <algorithm>

HeraArpeggiator::HeraArpeggiator()
{
    midiBuffer.ensureSize(8192);
    initializeNoteBuffer();
}

void HeraArpeggiator::reset()
{
    resetPending = true;
}

bool HeraArpeggiator::hasActiveNotes() const
{
    for (const ArpegChannel &ac : arpChannel) {
        if (ac.active())
            return true;
    }
    return false;
}

void HeraArpeggiator::setSampleRate(double newRate)
{
    if (currentSampleRate == newRate)
        return;

    currentSampleRate = newRate;
    reset();
}

void HeraArpeggiator::setTempo(double newTempo)
{
    beatsPerSecond = newTempo * (1.0 / 60.0);
}

void HeraArpeggiator::setEnabled(bool newEnabled)
{
    if (enabled == newEnabled)
        return;

    enabled = newEnabled;
    reset();
}

void HeraArpeggiator::setMode(int newMode)
{
    mode = newMode;
    if (newMode != kHeraArpModeUpDown) {
        int newDirection = (newMode == kHeraArpModeDown) ? -1 : +1;
        for (ArpegChannel &ch : arpChannel)
            ch.direction = newDirection;
    }
}

void HeraArpeggiator::setStep(int newStep)
{
    noteOnInterval = kHeraStepDurations[juce::jlimit(0, kHeraNumSteps - 1, newStep)];
}

void HeraArpeggiator::setRange(int newRange)
{
    octaves = std::max(1, newRange);
}

const juce::MidiBuffer &HeraArpeggiator::processBlock(const juce::MidiBuffer &inputMidi, int numSamples)
{
    juce::MidiBuffer &outputMidi = midiBuffer;

    if (!enabled && !resetPending)
        return inputMidi;

    outputMidi.clear();

    if (resetPending) {
        performReset(outputMidi, 0);
        resetPending = false;
    }

    if (!enabled) {
        outputMidi.addEvents(inputMidi, 0, -1, 0);
        return outputMidi;
    }

    int currentSamplePos = 0;
    juce::MidiBufferIterator it = inputMidi.begin();
    juce::MidiBufferIterator end = inputMidi.end();

    for (; it != end; ++it) {
        const juce::MidiMessageMetadata metadata = *it;

        tickArpeggio(outputMidi, metadata.samplePosition, metadata.samplePosition - currentSamplePos);
        currentSamplePos = metadata.samplePosition;

        juce::MidiMessage message;
        bool haveMessage = false;

        if (metadata.numBytes > 3)
            outputMidi.addEvent(metadata.data, metadata.numBytes, currentSamplePos);
        else {
            message = metadata.getMessage();
            haveMessage = true;
        }

        if (haveMessage) {
            if (message.isNoteOn())
                noteOn(outputMidi, currentSamplePos, message.getChannel(), message.getNoteNumber(), message.getVelocity());
            else if (message.isNoteOff())
                noteOff(outputMidi, currentSamplePos, message.getChannel(), message.getNoteNumber(), message.getVelocity());
            else if (message.isAllNotesOff() || message.isResetAllControllers())
                allNotesOff(outputMidi, currentSamplePos, message.getChannel());
            else if (message.isAllSoundOff())
                allSoundOff(outputMidi, currentSamplePos, message.getChannel());
            else
                outputMidi.addEvent(message, currentSamplePos);
        }
    }

    tickArpeggio(outputMidi, currentSamplePos, numSamples - currentSamplePos);

    return outputMidi;
}

void HeraArpeggiator::tickArpeggio(juce::MidiBuffer &outputMidi, int startSample, int numSamples)
{
    double beatsPerFrame = beatsPerSecond / currentSampleRate;
    double tickIncrement = beatsPerFrame / noteOnInterval;
    double tickPosition = currentTickPosition;

    for (int i = 0; i < numSamples; ++i) {
        double oldTickPosition = tickPosition;
        tickPosition += tickIncrement;
        if (oldTickPosition < 0.5 && tickPosition >= 0.5)
            sequenceNextNoteOff(outputMidi, startSample + i);
        if (oldTickPosition < 1.0 && tickPosition >= 1.0) {
            sequenceNextNoteOn(outputMidi, startSample + i);
            tickPosition -= (long)tickPosition;
        }
    }

    currentTickPosition = tickPosition;
}

void HeraArpeggiator::sequenceNextNoteOn(juce::MidiBuffer &outputMidi, int startSample)
{
    for (ArpegChannel &ac : arpChannel) {
        advanceToNextKey(ac);
        if (ac.hasPlayingKey) {
            ac.noteOffPlayingKey(outputMidi, startSample);
            ac.noteOnPlayingKey(outputMidi, startSample);
        }
    }
}

void HeraArpeggiator::sequenceNextNoteOff(juce::MidiBuffer &outputMidi, int startSample)
{
    for (ArpegChannel &ac : arpChannel) {
        if (ac.hasPlayingKey)
            ac.noteOffPlayingKey(outputMidi, startSample);
    }
}

void HeraArpeggiator::advanceToNextKey(ArpegChannel &ac)
{
    if (!ac.active())
        return;

    if (!ac.hasPlayingKey) {
        ac.playingKeyNoOctave = ac.initialKey;
        ac.hasPlayingKey = true;
        return;
    }

    jassert(ac.active());
    ArpegNoteList::iterator it = &ac.allNotes[ac.playingKeyNoOctave];
    ArpegNoteList &list = ac.activeNotes;

    ArpegNoteList::iterator first = list.begin();
    ArpegNoteList::iterator last = list.end();
    --last;

    switch (mode) {
    default:
    case kHeraArpModeUp:
        if (it != last)
            ++it;
        else {
            it = first;
            if (++ac.octave >= octaves)
                ac.octave = 0;
        }
        break;
    case kHeraArpModeDown:
        if (it != first)
            --it;
        else {
            it = last;
            if (--ac.octave <= -octaves)
                ac.octave = 0;
        }
        break;
    case kHeraArpModeUpDown: {
        bool atOctaveEnd = (ac.direction == +1) ? (it == last) : (it == first);
        bool mustSwitchDirection = atOctaveEnd &&
            ((ac.direction > 0) ? (ac.octave + 1 >= octaves) : (ac.octave - 1 <= -octaves));
        if (mustSwitchDirection)
            ac.direction = -ac.direction;
        if (ac.direction > 0) {
            if (it != last)
                ++it;
            else {
                it = first;
                ++ac.octave;
            }
        }
        else {
            if (it != first)
                --it;
            else {
                it = last;
                --ac.octave;
            }
        }
        break;
    }
    }

    ac.playingKeyNoOctave = it->key;
    ac.hasPlayingKey = true;
}

void HeraArpeggiator::noteOn(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1, int note, int velocity)
{
    ArpegChannel &ac = arpChannel[channelFrom1 - 1];
    ArpegNote &an = ac.allNotes[note];

    bool startClock = !hasActiveNotes();
    bool startChannel = !ac.active();

    noteOff(outputMidi, startSample, channelFrom1, note, velocity);
    jassert(!an.active);

    if (startClock) {
        // imminently trigger the next note, if none already playing
        currentTickPosition = std::nexttoward(1.0, 0.0);
    }
    if (startChannel) {
        ac.initialKey = note;
        ac.octave = 0;
        ac.direction = (mode == kHeraArpModeDown) ? -1 : +1;
        ac.hasPlayingKey = false;
    }

    an.active = true;
    an.playingVelocity = velocity;

    insertNoteInOrder(an, ac.activeNotes);
}

void HeraArpeggiator::noteOff(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1, int note, int velocity)
{
    ArpegChannel &ac = arpChannel[channelFrom1 - 1];
    ArpegNote &an = ac.allNotes[note];
    if (!an.active)
        return;

    if (ac.activeNotes.size() == 1) {
        jassert(ac.activeNotes.front().key == note);
        jassert(!ac.hasPlayingKey || ac.playingKeyNoOctave == note);
        ac.noteOffPlayingKey(outputMidi, startSample);
        ac.hasPlayingKey = false;
    }
    else if (ac.hasPlayingKey && ac.playingKeyNoOctave == note) {
        ac.noteOffPlayingKey(outputMidi, startSample);
        advanceToNextKey(ac);
        jassert(ac.hasPlayingKey && ac.playingKeyNoOctave != note);
    }

    jassert(!ac.hasPlayingKey || ac.playingKeyNoOctave != note);
    an.active = false;
    ac.activeNotes.erase(&an);
}

void HeraArpeggiator::allNotesOff(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1)
{
    ArpegChannel &ac = arpChannel[channelFrom1];
    outputMidi.addEvent(juce::MidiMessage::allNotesOff(channelFrom1), startSample);
    ac.hasPlayingKey = false;
    ac.isNoteOnPlayingKey = false;
    for (ArpegNote &note : ac.activeNotes)
        note.active = false;
    ac.activeNotes.clear();
}

void HeraArpeggiator::allSoundOff(juce::MidiBuffer &outputMidi, int startSample, int channelFrom1)
{
    ArpegChannel &ac = arpChannel[channelFrom1];
    outputMidi.addEvent(juce::MidiMessage::allSoundOff(channelFrom1), startSample);
    ac.hasPlayingKey = false;
    ac.isNoteOnPlayingKey = false;
    for (ArpegNote &note : ac.activeNotes)
        note.active = false;
    ac.activeNotes.clear();
}

void HeraArpeggiator::insertNoteInOrder(ArpegNote &an, ArpegNoteList &list)
{
    ArpegNoteList::iterator it = list.begin();
    ArpegNoteList::iterator end = list.end();

    auto less = [](const ArpegNote &a, const ArpegNote &b) -> bool {
        if (a.channelFrom1 != b.channelFrom1)
            return a.channelFrom1 < b.channelFrom1;
        return a.key < b.key;
    };

    for (; it != end && less(*it, an); ++it);
    list.insert(it, an);
}

void HeraArpeggiator::initializeNoteBuffer()
{
    for (int channelFrom1 = 1; channelFrom1 <= 16; ++channelFrom1) {
        ArpegChannel &ac = arpChannel[channelFrom1 - 1];
        ac.channelFrom1 = channelFrom1;
        for (int key = 0; key < 128; ++key) {
            ArpegNote &an = ac.allNotes[key];
            an.channelFrom1 = channelFrom1;
            an.key = key;
        }
    }
}

void HeraArpeggiator::performReset(juce::MidiBuffer &outputMidi, int startSample)
{
    for (ArpegChannel &ac : arpChannel) {
        outputMidi.addEvent(juce::MidiMessage::allNotesOff(ac.channelFrom1), startSample);
        ac.hasPlayingKey = false;
        ac.isNoteOnPlayingKey = false;
        for (ArpegNote &note : ac.activeNotes)
            note.active = false;
        ac.activeNotes.clear();
    }
}

void HeraArpeggiator::ArpegChannel::noteOnPlayingKey(juce::MidiBuffer &outputMidi, int startSample)
{
    jassert(hasPlayingKey);
    int playingKey = playingKeyNoOctave + 12 * octave;
    if (keyIsInMidiRange(playingKey)) {
        outputMidi.addEvent(juce::MidiMessage::noteOn(channelFrom1, playingKey, (juce::uint8)allNotes[playingKeyNoOctave].playingVelocity), startSample);
        isNoteOnPlayingKey = true;
    }
}

void HeraArpeggiator::ArpegChannel::noteOffPlayingKey(juce::MidiBuffer &outputMidi, int startSample)
{
    if (isNoteOnPlayingKey) {
        int playingKey = playingKeyNoOctave + 12 * octave;
        jassert(hasPlayingKey);
        outputMidi.addEvent(juce::MidiMessage::noteOff(channelFrom1, playingKey, (juce::uint8)allNotes[playingKeyNoOctave].playingVelocity), startSample);
        isNoteOnPlayingKey = false;
    }
}
