#include "file_io.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>

std::vector<uint8_t> FileIO::readFile(const std::string& fileName) {
    auto size = std::filesystem::file_size(fileName);
    std::vector<uint8_t> buffer(size);

    std::ifstream file(fileName, std::ios::binary);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}

void FileIO::writeFile(const std::string& fileName, const std::vector<uint8_t>& data) {
    std::ofstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + fileName);
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

