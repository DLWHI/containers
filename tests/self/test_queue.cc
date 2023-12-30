#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../s21_containers.h"
#include <queue>


TEST(front, test0) {
  s21::queue<int> test;
  test.push(1);
  test.push(2);

  std::queue<int> testS;
  testS.push(1);
  testS.push(2);
  ASSERT_EQ(test.front(), testS.front());
}

TEST(back, test0) {
  s21::queue<int> test;
  test.push(1);
  test.push(2);

  std::queue<int> testS;
  testS.push(1);
  testS.push(2);
  ASSERT_EQ(test.back(), testS.back());
}

TEST(empty, test0) { 
  s21::queue <int> test;
  test.push(1);
  test.pop();
  std::queue<int> testS;
  testS.push(1);
  testS.pop();
  ASSERT_EQ(test.empty(), testS.empty());
}

TEST(empty, test1) {
  s21::queue<int> test;
  std::queue<int> testS;
  ASSERT_EQ(test.empty(), testS.empty());
}

TEST(size, test1) {
  s21::queue<int> test;
  std::queue<int> testS;
  ASSERT_EQ(test.size(), testS.size());
}

TEST(swap, test1) {
  s21::queue<int> test;
  test.push(1);
  s21::queue<int> other;
  other.push(3);
  test.swap(other);
  std::queue<int> testS;
  testS.push(1);
  std::queue<int> otherS;
  otherS.push(3);
  testS.swap(otherS);
  ASSERT_EQ(test.back(), testS.back());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}