#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../containers/s21_red_black_tree.h"

TEST(RBTreeTest, insert_1) {
  s21::RedBlackTree<int, int> tree;
  tree.insert(3, 3);
  tree.insert(1, 1);
  tree.insert(5, 5);
  s21::RedBlackTree<int, int> true_tree;
  // Нужно придумать как сделать тесты...
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}