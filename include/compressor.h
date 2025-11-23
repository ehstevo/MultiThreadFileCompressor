#pragma once

#include <vector>
#include <cstdint>

class Compressor {
    public:

    virtual std::vector<uint8_t> compress(const std::vector<uint8_t>& chunk) = 0;
    virtual std::vector<uint8_t> decompress(const std::vector<uint8_t>& chunk) = 0;
};