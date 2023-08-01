#include <gtest/gtest.h>

#include <dlwhi/iterator.h>
#include <dlwhi/vector.h>

#include "test_helpers.h"

constexpr int constexpr_check() {
  constexpr int arr[5] = {1, 2, 3, 4, 5};
  dlwhi::iterator<const int, int*> ptr(&arr[0]);
  ptr += 3;
  return *ptr;
}

TEST(ItertorTest, test0) {
  EXPECT_EQ(4, constexpr_check());
}
