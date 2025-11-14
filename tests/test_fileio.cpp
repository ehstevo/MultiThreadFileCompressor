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

TEST(FileIOTest, ReadNonexistentFileThrows) {
    EXPECT_THROW(
        FileIO::readFile("no_such_file_999999.bin"),
        std::runtime_error
    );
}

TEST(FileIOTest, WriteAndReadEmptyFile) {
    std::string filename = "empty_test.bin";

    std::vector<uint8_t> emptyData;

    FileIO::writeFile(filename, emptyData);

    std::vector<uint8_t> result = FileIO::readFile(filename);

    EXPECT_TRUE(result.empty());

    std::remove(filename.c_str());
}

TEST(FileIOTest, WriteInvalidPathThrows) {
    std::string invalidPath = "/root/invalid_write_test.bin"; // non-writable for normal users

    std::vector<uint8_t> data = {10, 20, 30};

    EXPECT_THROW(
        FileIO::writeFile(invalidPath, data),
        std::runtime_error
    );
}

TEST(FileIOTest, OverwriteFile) {
    std::string filename = "overwrite_test.bin";

    std::vector<uint8_t> data1 = {1, 2, 3, 4, 5};
    std::vector<uint8_t> data2 = {99, 88, 77};

    // Write first version
    FileIO::writeFile(filename, data1);

    // Overwrite with second version
    FileIO::writeFile(filename, data2);

    // Read back
    std::vector<uint8_t> result = FileIO::readFile(filename);

    EXPECT_EQ(result.size(), data2.size());
    EXPECT_EQ(result, data2);

    std::remove(filename.c_str());
}
