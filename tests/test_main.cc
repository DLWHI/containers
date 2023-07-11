#include <gtest/gtest.h>
// #include "test_array.cc"
// #include "test_iterator.cc"
#include "test_vector.cc"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}