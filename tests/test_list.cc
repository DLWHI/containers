#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <list>
#include <random>

#include <dlwhi/list.h>
#include "test_helpers.h"

std::random_device ran_dev;
std::uniform_int_distribution<dlwhi::size_t> uid(1, 1000);
std::mt19937 gen(ran_dev());

#ifdef LOOP_COUNT
constexpr int loop = LOOP_COUNT;
#else
constexpr int loop = 50;
#endif

TEST(ListTest, ctor_default) {
  dlwhi::list<dummyCpy> lst;
  // EXPECT_EQ(lst.size(), 0);
  // EXPECT_EQ(lst.capacity(), 0);
  // EXPECT_EQ(lst.data(), nullptr);
  // EXPECT_THROW(lst.at(uid(gen)), std::out_of_range);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}