#include "HeraProgram.h"
#include "Pak.h"
#include "BinaryData.h"
#include <vector>
#include <memory>
#include <cstring>

void HeraProgram::setName(juce::StringRef newName)
{
    int copiedLength = juce::jmin<int>(newName.length(), sizeof(name) - 1);
    std::memcpy(name, newName, copiedLength);
    name[copiedLength] = '\0';
}

HeraProgram HeraProgram::fromValueTree(const juce::ValueTree &tree)
{
    HeraProgram program;
    const auto &parameters = HeraSynthesiser::getReferenceParameters();

    program.setName(getTreeName(tree));

    for (int p = 0; p < kHeraNumParameters; ++p) {
        const juce::RangedAudioParameter *param = parameters[p];
        program.values[p] = param->convertFrom0to1(param->getDefaultValue());
    }

    for (int c = 0, nc = tree.getNumChildren(); c < nc; ++c) {
        juce::ValueTree subtree = tree.getChild(c);
        if (!subtree.hasType("PARAM"))
            continue;

        juce::String id = subtree.getProperty("id").toString();
        juce::String value = subtree.getProperty("value").toString();

        if (id.isEmpty() || value.isEmpty())
            continue;

        int index = -1;
        for (int p = 0; index == -1 && p < kHeraNumParameters; ++p) {
            if (parameters[p]->paramID == id)
                index = p;
        }

        if (index != -1)
            program.values[index] = value.getFloatValue();
    }

    return program;
}

juce::ValueTree HeraProgram::toValueTree() const
{
    const auto &parameters = HeraSynthesiser::getReferenceParameters();

    juce::ValueTree tree("PARAMETERS");
    setTreeName(tree, name);

    for (int p = 0; p < kHeraNumParameters; ++p) {
        const juce::RangedAudioParameter *param = parameters[p];
        juce::ValueTree elt("PARAM");
        elt.setProperty("id", param->paramID, nullptr);
        elt.setProperty("value", values[p], nullptr);
        tree.addChild(elt, -1, nullptr);
    }

    return tree;
}

juce::String HeraProgram::toString() const
{
    return toValueTree().toXmlString();
}

//==============================================================================
juce::String HeraProgram::getTreeName(const juce::ValueTree &tree)
{
    return tree.getChildWithName("PROGRAM").getProperty("name").toString();
}

void HeraProgram::setTreeName(juce::ValueTree &tree, juce::String name)
{
    tree.getOrCreateChildWithName("PROGRAM", nullptr).setProperty("name", std::move(name), nullptr);
}

//==============================================================================
const HeraProgramList &HeraProgram::getPresets()
{
    static const HeraProgramList presets = loadPresets();
    return presets;
}

HeraProgramList HeraProgram::loadPresets()
{
    HeraProgramList presets;

    PakFileReader reader;
    reader.initWithData((const unsigned char *)BinaryData::presets_pak, BinaryData::presets_pakSize);
    uint32_t entryCount = reader.entryCount();

    bool entryExists = true;
    for (int presetNo = 0; entryExists; ++presetNo) {
        char fileNameBuf[32];
        std::sprintf(fileNameBuf, "Presets/Preset%03d.xml", presetNo);

        uint32_t entryIndex = 0;
        while (entryIndex < entryCount && reader.name(entryIndex) != fileNameBuf)
            ++entryIndex;
        entryExists = entryIndex != entryCount;

        if (entryExists) {
            std::string entryData = reader.extract(entryIndex);
            juce::ValueTree tree = juce::ValueTree::fromXml(entryData);
            presets.add(HeraProgram::fromValueTree(tree));
        }
    }

    presets.minimiseStorageOverheads();
    return presets;
}

bool HeraProgram::operator==(const HeraProgram &other) const noexcept
{
    return !std::memcmp(values, other.values, sizeof(values)) &&
        !std::strcmp(name, other.name);
}
