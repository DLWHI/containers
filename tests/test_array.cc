#include <gtest/gtest.h>

#include <sstream>
#include <random>

#include <dlwhi/array.h>

constexpr int constexpr_check(int val) {
  dlwhi::array<int, 7> arr1 = {1, 2, 3, 4, 5, 0, 0};
  dlwhi::array<int, 7> arr2 = {1, 2, 3, 4, 5, 6, 7};

  dlwhi::array<int, 7>& this_one = arr1.swap(arr2);
   arr2.fill(0);

  this_one[3] = val;

  return *(arr1.begin() + 3);
}

TEST(ArrayTest, init) {
  dlwhi::array<int, 7> arr;

  EXPECT_EQ(arr.size(), 7);
  EXPECT_EQ(arr.max_size(), 7);
  EXPECT_FALSE(arr.empty());
}

TEST(ArrayTest, no_element_init) {
  dlwhi::array<int, 0> arr;

  EXPECT_EQ(arr.size(), 0);
  EXPECT_EQ(arr.max_size(), 0);
  EXPECT_TRUE(arr.empty());
}

TEST(ArrayTest, zero_init) {
  dlwhi::array<int, 1000> arr{0};

  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(arr[i], 0);
}

TEST(ArrayTest, braced_list_init) {
  dlwhi::array<int, 7> arr{1, 2, 3, 4, 5, 6, 7};

  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(arr[i], i + 1);
}

TEST(ArrayTest, random_access) {
  dlwhi::array<int, 15> arr{0};
  for (int i = 0; i < arr.size(); i++) {
    EXPECT_EQ(arr[i], 0);
    arr[i] = i + 1;
  }
  for (int i = 0; i < arr.size(); i++) {
    EXPECT_EQ(arr[i], i + 1);
  }
}

TEST(ArrayTest, random_access_bounds) {
  dlwhi::array<int, 7> arr{0};
  for (int i = 0; i < arr.size(); i++) {
    EXPECT_EQ(arr.at(i), 0);
    arr.at(i) = i + 1;
  }
  for (int i = 0; i < arr.size(); i++) {
    EXPECT_EQ(arr.at(i), i + 1);
  }
  EXPECT_THROW(arr.at(-1), std::out_of_range);
  EXPECT_THROW(arr.at(arr.size()), std::out_of_range);
}

TEST(ArrayTest, front_back_access) {
  dlwhi::array<int, 7> arr{0};
  for (int i = 0; i < arr.size(); i++) {
    EXPECT_EQ(arr[i], 0);
    arr[i] = i + 1;
  }
  EXPECT_EQ(arr.front(), 1);
  EXPECT_EQ(arr.back(), arr.size());
}

TEST(ArrayTest, zero_element_access) {
  dlwhi::array<int, 0> arr;
  EXPECT_THROW(arr.at(4), std::out_of_range);
  EXPECT_THROW(arr.at(0), std::out_of_range);
}

TEST(ArrayTest, data_access) {
  dlwhi::array<int, 7> arr{1, 2, 3, 4, 5, 6, 7};
  int* ptr = arr.data();
  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(ptr[i], i + 1);
}

TEST(ArrayTest, comparison_1) {
  dlwhi::array<int, 7> arr1{1, 2, 3, 4, 5, 6, 7};
  dlwhi::array<int, 7> arr2{1, 2, 3, 4, 5, 6, 7};

  EXPECT_TRUE(arr1 == arr1);
  EXPECT_FALSE(arr1 != arr1);
  EXPECT_TRUE(arr1 == arr2);
  EXPECT_FALSE(arr1 != arr2);
}

TEST(ArrayTest, comparison_2) {
  dlwhi::array<int, 7> arr1{1, 2, 3, 4, 5, 6, 7};
  dlwhi::array<int, 7> arr2{7, 6, 5, 4, 3, 2, 1};

  EXPECT_FALSE(arr1 == arr2);
  EXPECT_TRUE(arr1 != arr2);
  EXPECT_FALSE(arr2 == arr1);
  EXPECT_TRUE(arr2 != arr1);
}

TEST(ArrayTest, comparison_self) {
  dlwhi::array<int, 7> arr{1, 2, 3, 4, 5, 6, 7};

  EXPECT_TRUE(arr == arr);
  EXPECT_FALSE(arr != arr);
}

TEST(ArrayTest, comparison_empty) {
  dlwhi::array<int, 0> arr1;
  dlwhi::array<int, 0> arr2;


  EXPECT_TRUE(arr1 == arr2);
  EXPECT_FALSE(arr1 != arr2);
}

TEST(ArrayTest, fill_1) {
  dlwhi::array<int, 7> arr;

  arr.fill(7);

  for (int n: arr)
    EXPECT_EQ(n, 7);
}

TEST(ArrayTest, fill_2) {
  dlwhi::array<int, 7> arr;

  arr.fill(0);

  for (auto it = arr.begin(); it != arr.end(); it++)
    EXPECT_EQ(*it, 0);
}

TEST(ArrayTest, fill_empty) {
  dlwhi::array<int, 0> arr;

  arr.fill(7);

  EXPECT_TRUE(arr.empty());
  EXPECT_EQ(arr.begin(), arr.end());
}

TEST(ArrayTest, make_array) {
  auto arr = dlwhi::make_array(1, 2, 3, 4, 5, 6, 7);

  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(arr[i], i + 1);
}
#ifndef _MSC_VER
TEST(ArrayTest, make_array_different_convertible) {
  auto arr = dlwhi::make_array(1u, 2u, 3u, 4, 5, 6l, 7l);

  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(arr[i], i + 1);
}
#endif
TEST(ArrayTest, make_array_constexpr_1) {
  constexpr auto arr = dlwhi::make_array(1, 2, 3, 4, 5, 6, 7);

  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(arr[i], i + 1);
}
#ifndef _MSC_VER
TEST(ArrayTest, make_array_constexpr_2) {
  constexpr auto arr = dlwhi::make_array(1u, 2u, 3u, 4, 5, 6l, 7l);

  for (int i = 0; i < arr.size(); i++)
    EXPECT_EQ(arr[i], i + 1);
}
#endif
TEST(ArrayTest, valid_constexpr_1) {
  constexpr dlwhi::array<int, 7> arr{7, 7, 7, 7, 7, 7, 7};

  for (auto it = arr.begin(); it != arr.end(); it++)
    EXPECT_EQ(*it, 7);
}

TEST(ArrayTest, valid_constexpr_2) {
  constexpr int cexper = constexpr_check(-7);
  EXPECT_EQ(cexper, -7);
}

TEST(ArrayTest, scenartio_stream) {
  dlwhi::array<int, 7> arr{1, 2, 3, 4, 5, 6, 7};
  std::stringstream stream;
  std::string expected("1 2 3 4 5 6 7");
  stream << arr;
  EXPECT_EQ(expected, stream.str());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
