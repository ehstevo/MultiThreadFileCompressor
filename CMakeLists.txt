cmake_minimum_required(VERSION 3.16)
project(MultiThreadCompressor VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
file(GLOB SRC_FILES src/*.cpp)

# Main executable
add_executable(MultiThreadCompressor ${SRC_FILES})

include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For older CMake versions
set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable installation" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()
add_subdirectory(tests)
