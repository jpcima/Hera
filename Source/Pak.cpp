// SPDX-License-Identifier: BSL-1.0

#include "Pak.h"
#include <juce_core/juce_core.h>

bool PakFileReader::initWithData(const uint8_t *data, uint32_t size)
{
    data_ = data;
    size_ = size;
    return readDictionary();
}

uint32_t PakFileReader::entryCount() const
{
    return entries_.size();
}

const std::string &PakFileReader::name(uint32_t nth) const
{
    const Entry &entry = entries_.at(nth);
    return entry.name;
}

std::string PakFileReader::extract(uint32_t nth) const
{
    const Entry &entry = entries_.at(nth);

    juce::MemoryInputStream memStream(data_ + contentOffset_, size_ - contentOffset_, false);
    juce::GZIPDecompressorInputStream gzStream(&memStream, false, juce::GZIPDecompressorInputStream::gzipFormat);

    if (!gzStream.setPosition(entry.offset))
        return std::string();

    std::string data(entry.size, '\0');
    unsigned sizeRead = (unsigned)gzStream.read(&data[0], entry.size);
    if (sizeRead != entry.size)
        return std::string();

    return data;
}

bool PakFileReader::readDictionary()
{
    entries_.clear();
    entries_.reserve(256);
    contentOffset_ = 0;

    const uint8_t *ptr = data_;
    uint32_t left = size_;
    bool dictionaryEnd = false;
    while (!dictionaryEnd) {
        Entry ent;

        if (left < 4)
            return false;

        ent.size = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
        ptr += 4;
        left -= 4;

        dictionaryEnd = ent.size == 0;

        if (!dictionaryEnd) {
            if (left < 4)
                return false;

            ent.offset = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
            ptr += 4;
            left -= 4;

            ent.name.reserve(64);
            char c;
            do {
                if (left == 0)
                    return false;
                c = *ptr++;
                --left;
                if (c != '\0')
                    ent.name.push_back(c);
            } while (c != '\0');
            entries_.push_back(std::move(ent));
        }
    }

    contentOffset_ = ptr - data_;
    return true;
}
