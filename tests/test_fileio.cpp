#include <gtest/gtest.h>
#include "file_io.h"
#include <filesystem>

TEST(FileIOTest, ReadWriteBinaryFile) {
    const std::string testFile = "test.bin";
    const std::vector<uint8_t> data = {1, 2, 3, 4, 5};

    // Write data
    FileIO::writeFile(testFile, data);

    // Read back
    std::vector<uint8_t> result = FileIO::readFile(testFile);

    EXPECT_EQ(result, data);

    // Cleanup
    std::filesystem::remove(testFile);
}
