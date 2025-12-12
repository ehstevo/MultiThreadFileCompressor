#pragma once

#include <vector>
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "compressor.h"
#include "huffman.h"
#include "chunker.h"
#include "file_io.h"

class ThreadedCompressor {
public:
    explicit ThreadedCompressor(std::unique_ptr<Compressor> comp, size_t chunkSize,
                                size_t thread_count = std::thread::hardware_concurrency());

    // High-level API
    std::vector<Compressor::EncodedData> compressFile(const std::string& input_path);

    std::vector<uint8_t> decompressFile(const std::vector<Compressor::EncodedData>& compressed);

private:
    // Core thread functionality
    void workerThread();

    // Task structure sent to workers
    struct Task {
        size_t chunk_index;
        std::vector<uint8_t> data;
        bool is_decompression;
    };

    // Result from each worker
    struct Result {
        size_t chunk_index;
        Compressor::EncodedData encoded;
        std::vector<uint8_t> decoded;
    };

    // Thread pool management
    size_t thread_count;
    std::vector<std::thread> workers_;
    size_t chunk_size;

    // Work queue
    std::queue<Task> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    bool shutdown_flag_ = false;

    // Completed results
    std::vector<std::optional<Result>> results_;
    std::mutex results_mutex_;

    // Compressor instance (Huffman)
    std::unique_ptr<Compressor> compressor;
};
