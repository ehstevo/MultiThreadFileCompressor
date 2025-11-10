#include <gtest/gtest.h>

// A simple sample test
TEST(SampleTest, BasicAssertions) {
    // Arrange
    int a = 5;
    int b = 5;

    // Act & Assert
    EXPECT_EQ(a, b);
    EXPECT_TRUE(a == b);
}
