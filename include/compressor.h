#pragma once

#include <vector>
#include <cstdint>

class Compressor {
    public:

    struct EncodedData {
        std::vector<uint8_t> bits;
        uint8_t padding;   // how many extra bits were added to final byte
    };

    virtual EncodedData compress(const std::vector<uint8_t>& chunk) = 0;
    virtual std::vector<uint8_t> decompress(EncodedData& chunk) = 0;
};