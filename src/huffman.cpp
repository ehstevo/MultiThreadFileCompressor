#include "huffman.h"
#include <functional>

// getters
HuffmanNode* Huffman::getRoot() {
    return root;
}

std::unordered_map<uint8_t, int> Huffman::getFrequencyTable() {
    return frequency_table;
}
    
std::unordered_map<uint8_t, std::string> Huffman::getHuffmanCodes() {
    return huffman_codes;
}

// build table mapping frequencies of each byte
void Huffman::buildFrequencyTable(const std::vector<uint8_t>& chunk) {
    frequency_table.clear();
    frequency_table.reserve(256);
    for (uint8_t b : chunk) {
        frequency_table[b]++;
    }
}

// builds huffman tree from frequency table according to huffman coding algorithm
void Huffman::buildHuffmanTree() {

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNode::Compare> min_heap;

    //Create leaf nodes and store them in owned_nodes
    owned_nodes.clear();

    for (const auto& [byte, freq] : frequency_table) {
        auto node = std::make_unique<HuffmanNode>(byte, freq);

        // raw pointer for the heap
        HuffmanNode* raw = node.get();
        min_heap.push(raw);

        // ownership goes into owned_nodes
        owned_nodes.push_back(std::move(node));
    }

    // Edge case: empty input → no tree
    if (min_heap.empty()) {
        root = nullptr;
        return;
    }

    // Build the Huffman tree
    while (min_heap.size() > 1) {
        HuffmanNode* left = min_heap.top(); 
        min_heap.pop();

        HuffmanNode* right = min_heap.top(); 
        min_heap.pop();

        auto parent = std::make_unique<HuffmanNode>(
            0, left->freq + right->freq
        );

        // Set children (using raw pointers)
        parent->left = left;
        parent->right = right;

        // Push parent node into heap
        HuffmanNode* raw_parent = parent.get();
        min_heap.push(raw_parent);

        // Parent takes ownership in owned_nodes
        owned_nodes.push_back(std::move(parent));
    }

    // The remaining element is the root
    root = min_heap.top();
}

void Huffman::generateCodes(HuffmanNode* node, std::string& current) {
    if (!node) return;

    // Leaf node: store the code
    if (!node->left && !node->right) {
        // handles 1 character case
        if (current.empty()) {
            current.push_back('0');
        }
        huffman_codes[node->byte] = current;
        return;
    }

    // Traverse left = add '0'
    current.push_back('0');
    generateCodes(node->left, current);
    current.pop_back();

    // Traverse right = add '1'
    current.push_back('1');
    generateCodes(node->right, current);
    current.pop_back();
}

EncodedData Huffman::encodeData(const std::vector<uint8_t>& chunk) {
    EncodedData result;

    uint8_t current_byte = 0;
    unsigned int bit_count = 0;

    // loop through all original bytes
    for (const auto& chunk_byte : chunk) {
        std::string bit_string = huffman_codes[chunk_byte];

        // compress original bytes according to huffman codes
        for (size_t i=0; i<bit_string.length(); ++i) {
            // byte is full so send to output vector
            if (bit_count==7) {
                if (bit_string[i]=='1') {
                    current_byte |= 1;
                }
                result.bits.push_back(current_byte);
                current_byte = 0;
                bit_count = 0;
            }
            // add low or high bit to compressed data and shift current_byte to next bit
            else {
                if (bit_string[i]=='1') {
                    current_byte |= 1;
                }
                current_byte <<= 1;
                bit_count++;
            }
        }
    }
    // leftover bits
    if (bit_count > 0) {
        // subtract 1 because current byte it always shifted one ahead of bit count
        current_byte <<= (8 - bit_count - 1);
        result.bits.push_back(current_byte);
        result.padding = 8 - bit_count;
    }
    return result;
}

// override compression interface functions
std::vector<uint8_t> Huffman::compress(const std::vector<uint8_t>& chunk) {
    return chunk;
}
std::vector<uint8_t> Huffman::decompress(const std::vector<uint8_t>& chunk) {
    return chunk;
}