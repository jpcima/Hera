// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <foleys_gui_magic/foleys_gui_magic.h>

//==============================================================================
/**
 */
class HeraAudioProcessorEditor : public foleys::MagicPluginEditor {
public:
    explicit HeraAudioProcessorEditor(foleys::MagicProcessorState &magicState);
    ~HeraAudioProcessorEditor() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeraAudioProcessorEditor)
};
