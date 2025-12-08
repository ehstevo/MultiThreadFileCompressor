#pragma once

#include "compressor.h"
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <iostream>

struct HuffmanNode {
    uint8_t byte;
    unsigned int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    // leaf node
    HuffmanNode(uint8_t b, size_t f)
                : byte(b), freq(f), left(nullptr), right(nullptr) {}

    struct Compare {
        bool operator()(const HuffmanNode* a,
                        const HuffmanNode* b) const {
                            return a->freq > b->freq;
                        }
    };
};

struct EncodedData {
    std::vector<uint8_t> bits;
    uint8_t padding;   // how many extra bits were added to final byte
};

class Huffman : public Compressor {
    public:

    Huffman() = default;
    ~Huffman() = default;
    // override compression interface functions
    virtual std::vector<uint8_t> compress(const std::vector<uint8_t>& chunk) override;
    virtual std::vector<uint8_t> decompress(const std::vector<uint8_t>& chunk) override;

    // build table mapping frequencies of each byte
    void buildFrequencyTable(const std::vector<uint8_t>& chunk);

    // builds huffman tree from frequency table according to huffman coding algorithm
    void buildHuffmanTree();

    // generates bitstrings for each bytes huffman code
    void generateCodes(HuffmanNode* node, std::string& current);

    // produces the compressed huffman coding of the uncompressed chunk
    EncodedData encodeData(const std::vector<uint8_t>& chunk);

    // transforms the compressed data back to the original based on the generated huffman codes
    std::vector<uint8_t> decodeData(EncodedData& data);

    void serializeTree(const HuffmanNode* node, std::vector<uint8_t>& out);
    std::unique_ptr<HuffmanNode> deserializeTree(const std::vector<uint8_t>& data, size_t& index);

    // getters
    HuffmanNode* getRoot();
    std::unordered_map<uint8_t, int> getFrequencyTable();
    std::unordered_map<uint8_t, std::string> getHuffmanCodes();

    private:

    HuffmanNode* root;
    std::vector<std::unique_ptr<HuffmanNode>> owned_nodes;
    std::unordered_map<uint8_t, int> frequency_table;
    std::unordered_map<uint8_t, std::string> huffman_codes;

};