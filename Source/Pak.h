// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <string>
#include <vector>
#include <cstdint>

class PakFileReader {
public:
    bool initWithData(const uint8_t *data, uint32_t size);
    uint32_t entryCount() const;

    const std::string &name(uint32_t nth) const;
    std::string extract(uint32_t nth) const;

private:
    struct Entry {
        uint32_t size = 0;
        uint32_t offset = 0;
        std::string name;
    };

    const uint8_t *data_ = nullptr;
    uint32_t size_ = 0;

    std::vector<Entry> entries_;
    size_t contentOffset_ = 0;

    bool readDictionary();
};
