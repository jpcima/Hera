#pragma once
#include "Hera.h"
#include <juce_core/juce_core.h>

class HeraProgram;
typedef juce::Array<HeraProgram> HeraProgramList;

class HeraProgram {
public:
    char name[256] = {};
    float values[kHeraNumParameters] = {};

    void setName(juce::StringRef newName);

    //==========================================================================
    static HeraProgram fromValueTree(const juce::ValueTree &tree);
    juce::ValueTree toValueTree() const;
    juce::String toString() const;

    //==========================================================================
    static juce::String getTreeName(const juce::ValueTree &tree);
    static void setTreeName(juce::ValueTree &tree, juce::String name);

    //==========================================================================
    static const HeraProgramList &getPresets();

    //==========================================================================
    bool operator==(const HeraProgram &other) const noexcept;
    bool operator!=(const HeraProgram &other) const noexcept { return !operator==(other); }

private:
    static HeraProgramList loadPresets();
};
