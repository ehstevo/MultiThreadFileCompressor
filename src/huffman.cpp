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
    std::vector<std::unique_ptr<HuffmanNode>> owned_nodes;
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

    // 3. The remaining element is the root
    root = min_heap.top();
}

// override compression interface functions
std::vector<uint8_t> Huffman::compress(const std::vector<uint8_t>& chunk) {
    return chunk;
}
std::vector<uint8_t> Huffman::decompress(const std::vector<uint8_t>& chunk) {
    return chunk;
}