#include "threaded_compressor.h"
#include <fstream>
#include <stdexcept>
#include <future>

ThreadedCompressor::ThreadedCompressor(std::unique_ptr<Compressor> comp, size_t chunkSize, size_t threadCount)
    : compressor(std::move(comp)), chunk_size(chunkSize), thread_count(threadCount) {}

std::vector<Compressor::EncodedData> ThreadedCompressor::compressFile(const std::string& path) {
    FileIO io;
    auto data = io.readFile(path);
    Chunker chunker(chunk_size);
    auto chunks = chunker.split(data);

    if (chunks.empty()) return {}; // empty file

    // Launch parallel compression tasks
    std::vector<std::future<Compressor::EncodedData>> futures;
    futures.reserve(chunks.size());

    for (auto& c : chunks) {
        futures.emplace_back(std::async(std::launch::async, [this, &c]() {
            return this->compressor->compress(c.data);
        }));
    }

    // Collect results
    std::vector<Compressor::EncodedData> output;
    for (auto& f : futures) {
        Compressor::EncodedData encoded = f.get();
        output.insert(output.end(), encoded);
    }

    return output;
}

std::vector<uint8_t> ThreadedCompressor::decompressFile(const std::vector<Compressor::EncodedData>& compressed) {
    // Decompression is single-threaded unless you implement chunk metadata.
    std::vector<uint8_t> output;
    for (auto d: compressed) {
        auto decompressed = compressor.get()->decompress(d);
        output.insert(output.end(), decompressed.begin(), decompressed.end());
    }
    return output;
}
