#include <gtest/gtest.h>
// #include "file_io.h"
#include "chunker.h"
#include <span>

TEST(ChunkerTest, SimpleSplit) {
    const std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Chunker chunker = Chunker(3);

    auto chunks = chunker.split(data);

    ASSERT_EQ(chunks.size(), 4);   // Expected 3,3,3,1
    EXPECT_EQ(chunks[0].data.size(), 3);
    EXPECT_EQ(chunks[1].data.size(), 3);
    EXPECT_EQ(chunks[2].data.size(), 3);
    EXPECT_EQ(chunks[3].data.size(), 1);

    // Check ordering of chunk IDs
    EXPECT_EQ(chunks[0].id, 0);
    EXPECT_EQ(chunks[1].id, 1);
    EXPECT_EQ(chunks[2].id, 2);
    EXPECT_EQ(chunks[3].id, 3);

    // Check actual data sliced correctly
    EXPECT_EQ(chunks[0].data[0], 1);
    EXPECT_EQ(chunks[1].data[0], 4);
    EXPECT_EQ(chunks[2].data[0], 7);
    EXPECT_EQ(chunks[3].data[0], 10);
}

TEST(ChunkerTests, PerfectlyDivisibleInput) {
    // Input: 12 bytes
    std::vector<uint8_t> data = {0,1,2,3,4,5,6,7,8,9,10,11};

    size_t chunkSize = 3;
    Chunker chunker(chunkSize);

    auto chunks = chunker.split(data);

    // Expect 4 chunks of size 3 each
    ASSERT_EQ(chunks.size(), 4);

    for (const auto& chunk : chunks) {
        EXPECT_EQ(chunk.data.size(), 3);
    }

    // Validate IDs are correct
    for (size_t i = 0; i < chunks.size(); i++) {
        EXPECT_EQ(chunks[i].id, i);
    }
}

TEST(ChunkerTests, EmptyInputProducesNoChunks) {
    std::vector<uint8_t> data;       // empty input
    size_t chunkSize = 1024;         // any size > 0

    Chunker chunker(chunkSize);
    auto chunks = chunker.split(data);

    EXPECT_TRUE(chunks.empty());
}

TEST(ChunkerTest, LargeInputSplitsCorrectly)
{
    const size_t chunkSize = 1000 * 1000; // 1 MB
    const size_t totalSize = 5 * 1000 * 1000 + 200 * 1000; // ~5.2 MB

    std::vector<uint8_t> data(totalSize, 0xAB); // Fill with dummy bytes

    Chunker chunker(chunkSize);
    auto chunks = chunker.split(data);

    // Expected number of chunks
    size_t expectedChunks = 6;

    EXPECT_EQ(chunks.size(), expectedChunks);

    // Check sizes of all but the last chunk
    for (size_t i = 0; i + 1 < chunks.size(); ++i)
    {
        EXPECT_EQ(chunks[i].data.size(), chunkSize);
    }

    // Last chunk size
    size_t expectedLast = totalSize % chunkSize;
    if (expectedLast == 0) {
        expectedLast = chunkSize;
    }

    EXPECT_EQ(chunks.back().data.size(), expectedLast);

    // Make sure chunk IDs are correct
    for (size_t i = 0; i < chunks.size(); ++i)
    {
        EXPECT_EQ(chunks[i].id, i);
    }
}

