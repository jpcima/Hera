// SPDX-License-Identifier: GPL-3.0-or-later

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
HeraAudioProcessorEditor::HeraAudioProcessorEditor(foleys::MagicProcessorState &magicState)
    : MagicPluginEditor(magicState, BinaryData::MagicUI_xml, BinaryData::MagicUI_xmlSize)
{
}

HeraAudioProcessorEditor::~HeraAudioProcessorEditor()
{
}
