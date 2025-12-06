#include <gtest/gtest.h>
#include "huffman.h"

TEST(HuffmanTest, BuildHuffmanTree_Basic) {
    Huffman compressor;

    // Prepare a simple input:
    // a: 5
    // b: 3
    // c: 2
    std::vector<uint8_t> data = {
        'a','a','a','a','a',
        'b','b','b',
        'c','c'
    };

    compressor.buildFrequencyTable(data);
    compressor.buildHuffmanTree();

    const HuffmanNode* root = compressor.getRoot();
    // std::cout << compressor.getRoot()->freq << std::endl;
    ASSERT_NE(root, nullptr) << "Root node should not be null.";

    // Total frequency should match input size: 10
    EXPECT_EQ(root->freq, 10);

    // Since there are 3 unique symbols, root should not be a leaf
    EXPECT_TRUE(root->left != nullptr || root->right != nullptr);

    // Collect leaves recursively
    std::unordered_map<uint8_t, int> leaves;

    std::function<void(const HuffmanNode*)> collect = [&](const HuffmanNode* node) {
        if (!node) return;
        if (!node->left && !node->right) {
            leaves[node->byte] = node->freq;
            return;
        }
        collect(node->left);
        collect(node->right);
    };

    collect(root);

    // Ensure leaf frequencies match expectation
    ASSERT_EQ(leaves.size(), 3);
    EXPECT_EQ(leaves['a'], 5);
    EXPECT_EQ(leaves['b'], 3);
    EXPECT_EQ(leaves['c'], 2);
}

TEST(HuffmanTest, SingleSymbolGeneratesSingleCode) {
    Huffman h;
    std::vector<uint8_t> input = { 'A', 'A', 'A', 'A' };

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto codes = h.getHuffmanCodes();

    ASSERT_EQ(codes.size(), 1);
    ASSERT_EQ(codes.count('A'), 1u);

    // Code must not be empty
    EXPECT_FALSE(codes['A'].empty());
    ASSERT_EQ(codes['A'],"0");
}

TEST(HuffmanTest, TwoSymbolsGenerateTwoCodes) {
    Huffman h;
    std::vector<uint8_t> input = { 'A', 'B', 'A', 'B', 'A', 'B' };

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto codes = h.getHuffmanCodes();

    ASSERT_EQ(codes.size(), 2);
    ASSERT_TRUE(codes.count('A'));
    ASSERT_TRUE(codes.count('B'));

    EXPECT_FALSE(codes['A'].empty());
    EXPECT_FALSE(codes['B'].empty());

    // Codes must be different
    EXPECT_NE(codes['A'], codes['B']);
}

TEST(HuffmanTest, MultipleSymbolsGenerateCodesForAll) {
    Huffman h;
    std::vector<uint8_t> input = { 'A','B','C','D','E','F','G' };

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto codes = h.getHuffmanCodes();

    ASSERT_EQ(codes.size(), 7);

    for (char c = 'A'; c <= 'G'; c++) {
        EXPECT_TRUE(codes.count(c)) << "Missing code for " << c;
        EXPECT_FALSE(codes[c].empty()) << "Code for " << c << " is empty!";
    }
}

TEST(HuffmanTest, NoCodeIsPrefixOfAnotherCode) {
    Huffman h;
    std::vector<uint8_t> input = { 'A','A','B','C','C','C','D','D','D','D' };

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto codes = h.getHuffmanCodes();

    for (auto& [symbol1, code1] : codes) {
        for (auto& [symbol2, code2] : codes) {
            if (symbol1 == symbol2) continue;

            // code1 should not start with code2
            EXPECT_FALSE(code1.rfind(code2, 0) == 0)
                << "Code for " << symbol1 << " (" << code1 
                << ") is prefixed by code for " << symbol2 << " (" << code2 << ")";
        }
    }
}

TEST(HuffmanTest, SkewedFrequenciesProduceCorrectTreeDepths) {
    Huffman h;
    std::vector<uint8_t> input;

    // A appears many times, others appear once
    input.insert(input.end(), 100, 'A');
    input.push_back('B');
    input.push_back('C');
    input.push_back('D');

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto codes = h.getHuffmanCodes();

    EXPECT_LT(codes['A'].size(), codes['B'].size());
    EXPECT_LT(codes['A'].size(), codes['C'].size());
    EXPECT_LT(codes['A'].size(), codes['D'].size());
}

