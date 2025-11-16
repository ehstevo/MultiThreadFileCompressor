#include "chunker.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

std::vector<Chunk> Chunker::split(const std::vector<uint8_t>& input) {
    if (input.empty()) {
        throw std::runtime_error("Input file is empty");
    }
    size_t input_size = input.size();
    size_t num_chunks = std::ceil(input_size / chunk_size); // number of chunks to create
    std::vector<Chunk> chunks;

    for (size_t i=0; i<num_chunks; i++) {
        Chunk temp;
        temp.id = i;
        std::vector<uint8_t> chunk_data;
        size_t offset = chunk_size*i;
        // if last chunk, copy rest of contents
        if (i+1==num_chunks) {
            std::copy(input.begin()+offset, input.end(), std::back_inserter(chunk_data));
        }
        // copy chunk size
        else {
            std::copy(input.begin()+offset, input.begin()+(offset+chunk_size), std::back_inserter(chunk_data));
        }
        temp.data = chunk_data;
        temp.original_size = chunk_data.size();
        chunks.push_back(temp);
    }

    return chunks;
}