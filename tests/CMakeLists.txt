# Add test executable
file(GLOB TEST_SRC_FILES *.cpp)
add_executable(runTests ${TEST_SRC_FILES})

# Link test executable with GoogleTest and main project
target_link_libraries(runTests
    PRIVATE
    core
    gtest
    gtest_main
    pthread
)

# Include headers
target_include_directories(runTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Discover tests automatically
include(GoogleTest)
gtest_discover_tests(runTests)
