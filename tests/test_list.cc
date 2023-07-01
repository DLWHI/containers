#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include "../s21_containers.h"
#include <list>


TEST(paramConstr, test0) {
  s21::list<int> test(0);
  test.insert(test.begin(), 1);
  std::list<int> testS(0);
  testS.insert(testS.begin(), 1);
  ASSERT_EQ(*test.begin(), *testS.begin());
}

TEST(paramConstr, test1) {
  s21::list<int> test(1);
  test.insert(test.begin(), 1);
  std::list<int> testS(1);
  testS.insert(testS.begin(), 1);
  ASSERT_EQ(*test.begin(), *testS.begin());
}

TEST(copyConstr, test0) {
  s21::list<int> test(4);
  s21::list<int> copy(test);
  ASSERT_EQ(*test.begin(), *copy.begin());
}

TEST(counter, test0) {
  s21::list<int> test(3);
  test.insert(test.begin(), 1);
  std::list<int> testS(3);
  testS.insert(testS.begin(), 1);
  ASSERT_EQ(test.size(), testS.size());
}

TEST(front, test0) {
  s21::list<int> test(3);
  test.insert(test.begin(), 1);
  std::list<int> testS(3);
  testS.insert(testS.begin(), 1);
  ASSERT_EQ(test.front(), testS.front());
}

TEST(empty, test0) {
  s21::list<int> test(3);
  std::list<int> testS(3);
  ASSERT_EQ(test.empty(), testS.empty());
}

TEST(empty, test1) {
  s21::list<int> test(0);
  std::list<int> testS(0);
  ASSERT_EQ(test.empty(), testS.empty());
}

TEST(back, test1) {
  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.push_front(3);
  std::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.push_front(3);
  ASSERT_EQ(test.back(), testS.back());
  std::cout << std::endl << test.back() << std::endl << testS.back() ;
}

TEST(pushFront, test0) {
  s21::list<int> test(3);
  test.push_front(1);
  std::list<int> testS(3);
  testS.push_front(1);
  ASSERT_EQ(test.size(), testS.size());
}

TEST(pushFront, test1) {
  s21::list<int> test(3);
  test.push_front(7);
  std::list<int> testS(3);
  testS.push_front(7);
  ASSERT_EQ(test.front(), testS.front());
}

TEST(pushBack, test0) {
  s21::list<int> test(3);
  test.push_back(1);
  std::list<int> testS(3);
  testS.push_back(1);
  ASSERT_EQ(test.size(), testS.size());
}

TEST(pushBack, test1) {
  s21::list<int> test(0);
  test.push_back(3);
  std::list<int> testS(0);
  testS.push_back(3);
  ASSERT_EQ(test.back(), testS.back());
  std::cout << std::endl << test.back() << std::endl << testS.back() ;
}

TEST(erase, test0) {
  s21::list<double> test(0);
  test.push_back(1);
  test.push_back(3);
  test.erase(test.begin());
  std::list<double> testS(0);
  testS.push_back(1);
  testS.push_back(3);
  testS.erase(testS.begin());
  ASSERT_EQ(test.front(), testS.front());
  std::cout << std::endl << test.front() << std::endl << testS.front() ;
}

TEST(erase, test1) {
  s21::list<double> test(0);
  test.push_back(1);
  test.push_back(6);
  test.erase(test.begin());
  std::list<double> testS(0);
  testS.push_back(1);
  testS.push_back(6);
  testS.erase(testS.begin());
  ASSERT_EQ(test.size(), testS.size());
  std::cout << std::endl << test.front() << std::endl << testS.front() ;
}

TEST(popBack, test0) {
  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.push_front(3);
  test.pop_back();
  std::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.push_front(3);
  testS.pop_back();
  ASSERT_EQ(test.back(), testS.back());
  std::cout << std::endl << test.back() << std::endl << testS.back();
}

TEST(popBack, test1) {
  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.pop_back();
  std::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.pop_back();
  ASSERT_EQ(test.size(), testS.size());
  std::cout << std::endl << test.back() << std::endl << testS.back();
}

TEST(popFront, test0) {
  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.pop_front();
  std::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.pop_front();
  ASSERT_EQ(test.front(), testS.front());
  std::cout << std::endl << test.front() << std::endl << testS.front();
}

TEST(popFront, test1) {
  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.pop_front();
  std::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.pop_front();
  ASSERT_EQ(test.size(), testS.size());
  std::cout << std::endl << test.front() << std::endl << testS.front();
}

TEST(reverse, test0) {
  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.push_front(3);
  test.reverse();
  std::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.push_front(3);
  testS.reverse();
  ASSERT_EQ(test.back(), testS.back());
}

TEST(reverse, test1) {
  s21::list<int> test(0);
  test.push_front(4);
  test.push_front(3);
  test.push_front(2);
  test.push_front(1);
  test.reverse();

  std::list<int> testS(0);
  testS.push_front(4);
  testS.push_front(3);
  testS.push_front(2);
  testS.push_front(1);
  testS.reverse();
  ASSERT_EQ(test.front(), testS.front());
}

TEST(reverse, test2) {
  s21::list<int> test(0);
  test.push_front(4);
  test.push_front(3);
  test.push_front(2);
  test.push_front(1);
  test.reverse();

  s21::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.push_front(3);
  testS.push_front(4);
  ASSERT_EQ(test.IsEqual(testS), 1);
}

TEST(merge, test0) {
  s21::list<int> test(0);
  test.push_front(10);
  test.push_front(7);
  s21::list<int> testOne(0);
  testOne.push_front(6);
  testOne.push_front(3);
  std::cout << test;

  test.merge(testOne);

  std::cout << test;

  std::list<int> testS(0);
  testS.push_front(10);
  testS.push_front(7);
  std::list<int> testSOne(0);
  testSOne.push_front(6);
  testSOne.push_front(3);

  testS.merge(testSOne);

  ASSERT_EQ(test.front(), testS.front());
}

TEST(merge, test1) {
  s21::list<int> test(0);
  test.push_front(10);
  test.push_front(1);
  s21::list<int> testOne(0);
  testOne.push_front(11);
  testOne.push_front(3);

  test.merge(testOne);

  std::list<int> testS(0);
  testS.push_front(10);
  testS.push_front(1);
  std::list<int> testSOne(0);
  testSOne.push_front(11);
  testSOne.push_front(3);

  testS.merge(testSOne);

  ASSERT_EQ(test.front(), testS.front());
}

TEST(swap, test0) {
s21::list<int> test(0);
test.push_front(10);
test.push_front(9);
test.push_front(8);
test.push_front(7);
s21::list<int> testOne(0);
testOne.push_front(6);
testOne.push_front(5);
testOne.push_front(4);
testOne.push_front(3);

test.swap(testOne);

std::list<int> testS(0);
testS.push_front(10);
testS.push_front(9);
testS.push_front(8);
testS.push_front(7);
std::list<int> testSOne(0);
testSOne.push_front(6);
testSOne.push_front(5);
testSOne.push_front(4);
testSOne.push_front(3);

testS.swap(testSOne);

ASSERT_EQ(test.front(), testS.front());
ASSERT_EQ(test.back(), testS.back());
}

TEST(splice, test0) {
s21::list<int> test(0);
test.push_front(10);
test.push_front(9);
test.push_front(8);
test.push_front(7);
s21::list<int> testOne(0);
testOne.push_front(6);
testOne.push_front(5);
testOne.push_front(4);
testOne.push_front(3);

test.splice(test.begin(), testOne);

std::list<int> testS(0);
testS.push_front(10);
testS.push_front(9);
testS.push_front(8);
testS.push_front(7);
std::list<int> testSOne(0);
testSOne.push_front(6);
testSOne.push_front(5);
testSOne.push_front(4);
testSOne.push_front(3);

testS.splice(testS.begin(), testSOne);
ASSERT_EQ(test.back(), testS.back());



}

TEST(wierd, test0) {
  s21::list<int> test{1, 2, 3, 4, 5, 6, 6, 6};
  EXPECT_EQ(test.max_size(), std::numeric_limits<s21::list<int>::size_type>::max());
  test.unique();
  int counter = 1;
  for (auto el: test) {
    EXPECT_EQ(el, counter++);
  }

  auto it = test.insert(test.end(), 7);
  EXPECT_EQ(test.back(), 7);
  auto moved_it = std::move(it);
  test.erase(moved_it);
  EXPECT_EQ(*--test.end(), 6);
}

TEST(unique, test0) {

  s21::list<int> test(0);
  test.push_front(9);
  test.push_front(9);
  test.push_front(9);
  test.push_front(9);
  test.unique();

  s21::list<int> testS(0);
  testS.push_front(9);
  ASSERT_EQ(test.IsEqual(testS), 1);

}

TEST(unique, test1) {

  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.push_front(3);
  test.push_front(9);
  test.unique();

  s21::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.push_front(3);
  testS.push_front(9);

  ASSERT_EQ(test.IsEqual(testS), 1);
}

TEST(comparison, test0) {

  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.push_front(3);
  test.push_front(9);

  s21::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(2);
  testS.push_front(3);
  testS.push_front(9);
  
  ASSERT_EQ(test.IsEqual(testS), 1);

}

TEST(comparison, test1) {

  s21::list<int> test(0);
  test.push_front(1);
  test.push_front(2);
  test.push_front(3);
  test.push_front(9);

  s21::list<int> testS(0);
  testS.push_front(1);
  testS.push_front(0);
  testS.push_front(3);
  testS.push_front(9);
  
  ASSERT_EQ(test.IsEqual(testS), 0);
}

TEST(comparison, test2) {
  s21::list<int> test(0);
  s21::list<int> testS(0);
  testS.push_front(0); 
  ASSERT_EQ(test.IsEqual(testS), 0);
}

TEST(comparison, test3) {
  s21::list<int> test(0);
  s21::list<int> testS(0);
  ASSERT_EQ(test.IsEqual(testS), 1);
}

// TEST(sort, test0) {

//   s21::list<int> test(0);
//   test.push_front(1);
//   test.push_front(2);
//   test.push_front(3);
//   test.push_front(9);
//   test.sort();

//   s21::list<int> testS(0);
//   testS.push_front(1);
//   testS.push_front(0);
//   testS.push_front(3);
//   testS.push_front(9);
  
//   ASSERT_EQ(test.IsEqual(testS), 0);
// }


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}