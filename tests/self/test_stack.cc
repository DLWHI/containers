#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../s21_containers.h"
#include <stack>

TEST(top, test0) {
  s21::stack<int> test;
  test.push(1);
  test.push(2);

  std::stack<int> testS;
  testS.push(1);
  testS.push(2);
  ASSERT_EQ(test.top(), testS.top());
}

TEST(pop, test0) {
  s21::stack<int> test;
  test.push(1);
  test.push(2);
  test.push(3);
  test.pop();
  std::stack<int> testS;
  testS.push(1);
  testS.push(2);
  testS.push(3);
  testS.pop();
  ASSERT_EQ(test.top(), testS.top());
}

TEST(empty, test1) {
  s21::stack<int> test;
  std::stack<int> testS;
  ASSERT_EQ(test.empty(), testS.empty());
}

TEST(size, test1) {
  s21::stack<int> test;
  std::stack<int> testS;
  ASSERT_EQ(test.size(), testS.size());
}

TEST(swap, test1) {
  s21::stack<int> test;
  test.push(1);
  s21::stack<int> other;
  other.push(3);
  test.swap(other);
  std::stack<int> testS;
  testS.push(1);
  std::stack<int> otherS;
  otherS.push(3);
  testS.swap(otherS);
  ASSERT_EQ(test.top(), testS.top());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}