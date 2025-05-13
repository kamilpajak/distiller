#include <gtest/gtest.h>

// Basic sanity test to make sure the tests run
TEST(DistillerTests, SanityCheck) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
