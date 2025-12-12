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

TEST(HuffmanTest, EncodeSimpleString) {
    std::vector<uint8_t> data = { 'A', 'A', 'B' };
    Huffman h;

    h.buildFrequencyTable(data);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto encoded = h.encodeData(data);
    // std::cout << static_cast<int>(encoded.bits[0]) << std::endl;

    // The original is 3 bytes = 24 bits
    // The encoded version should be 3 bits → packed into 1 byte.
    ASSERT_EQ(encoded.bits.size(), 1);
    ASSERT_EQ(encoded.padding, 5);

    uint8_t byte = encoded.bits[0];

    // Code mappings may differ between A=0/B=1 or A=1/B=0.
    // So check "either case".
    bool case1 = ((byte & 0b11100000) >> 5) == 0b001; // A=0,A=0,B=1 pattern
    bool case2 = ((byte & 0b11100000) >> 5) == 0b110; // A=1,A=1,B=0 pattern

    ASSERT_TRUE(case1 || case2);
}

TEST(HuffmanTest, EncodeSingleSymbol) {
    std::vector<uint8_t> data = { 'A','A','A','A','A','A' };
    Huffman h;

    h.buildFrequencyTable(data);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto encoded = h.encodeData(data);

    // 6 bits → still fits in 1 byte
    ASSERT_EQ(encoded.bits.size(), 1);
    ASSERT_EQ(encoded.padding, 2);

    // That byte should have first 6 bits = either 000000 or 111111
    uint8_t byte = encoded.bits[0];

    bool allZeros = ((byte >> 2) == 0x00);
    bool allOnes  = ((byte >> 2) == 0x3F);

    ASSERT_TRUE(allZeros || allOnes);
}

TEST(HuffmanTest, EncodeLongerString) {
    std::vector<uint8_t> data = { 'A','B','C','A','B','C','A','B','C' };
    Huffman h;

    h.buildFrequencyTable(data);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto encoded = h.encodeData(data);

    ASSERT_FALSE(encoded.bits.empty());
    ASSERT_LT(encoded.bits.size(), data.size());
}

TEST(HuffmanTest, EncodeEmptyInput) {
    std::vector<uint8_t> data = {};
    Huffman h;

    auto encoded = h.encodeData(data);

    ASSERT_TRUE(encoded.bits.empty());
}

TEST(HuffmanTest, BasicRoundTrip) {
    Huffman h;

    std::vector<uint8_t> input = {
        'a','b','a','a','c','b','d','d','d'
    };

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);  

    auto encoded = h.encodeData(input);
    auto decoded = h.decodeData(encoded);

    ASSERT_EQ(decoded, input);
}

TEST(HuffmanTest, SingleCharacterInput) {
    Huffman h;

    std::vector<uint8_t> input(100, 'x'); // 100 x's

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto encoded = h.encodeData(input);
    auto decoded = h.decodeData(encoded);

    ASSERT_EQ(decoded, input);
}

TEST(HuffmanTest, RandomDataRoundTrip) {
    Huffman h;
    std::vector<uint8_t> input(10'000);

    for (size_t i = 0; i < input.size(); ++i) {
        input[i] = static_cast<uint8_t>(rand() % 256);
    }

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto encoded = h.encodeData(input);
    auto decoded = h.decodeData(encoded);

    ASSERT_EQ(decoded, input);
}

TEST(HuffmanTest, HandlesNonByteAlignedData) {
    Huffman h;

    std::vector<uint8_t> input = {'a','b','c'};

    h.buildFrequencyTable(input);
    h.buildHuffmanTree();
    std::string start;
    h.generateCodes(h.getRoot(), start);

    auto encoded = h.encodeData(input);
    auto decoded = h.decodeData(encoded);

    ASSERT_EQ(decoded, input);
}

TEST(HuffmanTest, RandomDataRoundTripFullTest) {
    Huffman h;
    std::vector<uint8_t> input(10'000);

    for (size_t i = 0; i < input.size(); ++i) {
        input[i] = static_cast<uint8_t>(rand() % 256);
    }

    auto encoded = h.compress(input);
    auto decoded = h.decompress(encoded);

    ASSERT_EQ(decoded, input);
}