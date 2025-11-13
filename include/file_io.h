#pragma once

#include <vector>
#include <string>
#include <cstdint>

class FileIO {
    public:

    // reads file into a byte vector
    static std::vector<uint8_t> readFile(const std::string& fileName);

    // writes byte vector into a file
    static void writeFile(const std::string& fileName, const std::vector<uint8_t>& data);
};