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

    const auto* root = compressor.getRoot();
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
