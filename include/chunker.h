#pragma once

#include <vector>
#include <cstdint>

struct Chunk {
    unsigned int id;
    std::vector<uint8_t> data;
    size_t original_size;
};

class Chunker {
    public:
    
    Chunker(size_t chunk_size)
    : chunk_size(chunk_size) {

    };

    // splits original file into chunks for paralleled compression
    std::vector<Chunk> split(const std::vector<uint8_t>& input);

    private:
    size_t chunk_size;
};