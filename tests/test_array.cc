#include <gtest/gtest.h>
#include <sp/array.h>

#include <random>
#include <sstream>

template <typename T, int64_t N>
using TargetArray = sp::array<T, N>;

constexpr int constexpr_check(int val) {
  TargetArray<int, 7> arr1 = {1, 2, 3, 4, 5, 0, 0};
  TargetArray<int, 7> arr2 = {1, 2, 3, 4, 5, 6, 7};

  arr1.swap(arr2);
  arr2.fill(0);

  arr2[3] = val;

  return *(arr2.begin() + 3);
}

TEST(ArrayTest, init) {
  TargetArray<int, 7> arr{1, 2, 3, 4, 5, 6, 7};
  ASSERT_EQ(arr.size(), 7);
  ASSERT_EQ(arr.max_size(), 7);
  ASSERT_FALSE(arr.empty());
}

TEST(ArrayTest, no_element_init) {
  TargetArray<int, 0> arr;

  ASSERT_EQ(arr.size(), 0);
  ASSERT_EQ(arr.max_size(), 0);
  ASSERT_TRUE(arr.empty());
}

TEST(ArrayTest, zero_init) {
  TargetArray<int, 1000> arr{0};

  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(arr[i], 0);
}

TEST(ArrayTest, braced_list_init) {
  TargetArray<int, 7> arr{1, 2, 3, 4, 5, 6, 7};

  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(arr[i], i + 1);
}

TEST(ArrayTest, random_access) {
  TargetArray<int, 15> arr{0};
  for (int i = 0; i < arr.size(); i++) {
    ASSERT_EQ(arr[i], 0);
    arr[i] = i + 1;
  }
  for (int i = 0; i < arr.size(); i++) {
    ASSERT_EQ(arr[i], i + 1);
  }
}

TEST(ArrayTest, random_access_bounds) {
  TargetArray<int, 7> arr{0};
  for (int i = 0; i < arr.size(); i++) {
    ASSERT_EQ(arr.at(i), 0);
    arr.at(i) = i + 1;
  }
  for (int i = 0; i < arr.size(); i++) {
    ASSERT_EQ(arr.at(i), i + 1);
  }
  ASSERT_THROW(arr.at(-1), std::out_of_range);
  ASSERT_THROW(arr.at(arr.size()), std::out_of_range);
}

TEST(ArrayTest, front_back_access) {
  TargetArray<int, 7> arr{0};
  for (int i = 0; i < arr.size(); i++) {
    ASSERT_EQ(arr[i], 0);
    arr[i] = i + 1;
  }
  ASSERT_EQ(arr.front(), 1);
  ASSERT_EQ(arr.back(), arr.size());
}

TEST(ArrayTest, zero_element_access) {
  TargetArray<int, 0> arr;
  ASSERT_THROW(arr.at(4), std::out_of_range);
  ASSERT_THROW(arr.at(0), std::out_of_range);
}

TEST(ArrayTest, data_access) {
  TargetArray<int, 7> arr{1, 2, 3, 4, 5, 6, 7};
  int* ptr = arr.data();
  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(ptr[i], i + 1);
}

TEST(ArrayTest, comparison_1) {
  TargetArray<int, 7> arr1{1, 2, 3, 4, 5, 6, 7};
  TargetArray<int, 7> arr2{1, 2, 3, 4, 5, 6, 7};

  ASSERT_TRUE(arr1 == arr1);
  ASSERT_FALSE(arr1 != arr1);
  ASSERT_TRUE(arr1 == arr2);
  ASSERT_FALSE(arr1 != arr2);
}

TEST(ArrayTest, comparison_2) {
  TargetArray<int, 7> arr1{1, 2, 3, 4, 5, 6, 7};
  TargetArray<int, 7> arr2{7, 6, 5, 4, 3, 2, 1};

  ASSERT_FALSE(arr1 == arr2);
  ASSERT_TRUE(arr1 != arr2);
  ASSERT_FALSE(arr2 == arr1);
  ASSERT_TRUE(arr2 != arr1);
}

TEST(ArrayTest, comparison_self) {
  TargetArray<int, 7> arr{1, 2, 3, 4, 5, 6, 7};

  ASSERT_TRUE(arr == arr);
  ASSERT_FALSE(arr != arr);
}

TEST(ArrayTest, comparison_empty) {
  TargetArray<int, 0> arr1;
  TargetArray<int, 0> arr2;

  ASSERT_TRUE(arr1 == arr2);
  ASSERT_FALSE(arr1 != arr2);
}

TEST(ArrayTest, fill_1) {
  TargetArray<int, 7> arr;

  arr.fill(7);

  for (int n : arr) ASSERT_EQ(n, 7);
}

TEST(ArrayTest, fill_2) {
  TargetArray<int, 7> arr;

  arr.fill(0);

  for (auto it = arr.begin(); it != arr.end(); it++) ASSERT_EQ(*it, 0);
}

TEST(ArrayTest, fill_empty) {
  TargetArray<int, 0> arr;

  arr.fill(7);

  ASSERT_TRUE(arr.empty());
  ASSERT_EQ(arr.begin(), arr.end());
}

TEST(ArrayTest, to_array) {
  auto arr = sp::to_array({1, 2, 3, 4, 5, 6, 7});

  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(arr[i], i + 1);
}

TEST(ArrayTest, to_array_custom_order) {
  auto arr =
      sp::to_array({1, 2, 3, 4, 5, 6, 7},
                   std::integer_sequence<int64_t, 6, 5, 4, 3, 2, 1, 0>{});

  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(arr[i], 7 - i);
}

TEST(ArrayTest, to_array_copy) {
  auto arr =
      sp::to_array({1, 2, 3, 4, 5, 6, 7},
                   std::integer_sequence<int64_t, 6, 6, 6, 6, 6, 6, 6>{});

  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(arr[i], 7);
}

TEST(ArrayTest, to_array_constexpr) {
  constexpr auto arr = sp::to_array({1, 2, 3, 4, 5, 6, 7});

  for (int i = 0; i < arr.size(); i++) ASSERT_EQ(arr[i], i + 1);
}

TEST(ArrayTest, valid_constexpr_1) {
  constexpr TargetArray<int, 7> arr{7, 7, 7, 7, 7, 7, 7};

  for (auto it = arr.begin(); it != arr.end(); it++) ASSERT_EQ(*it, 7);
}

TEST(ArrayTest, valid_constexpr_2) {
  constexpr int cexper = constexpr_check(-7);
  ASSERT_EQ(cexper, -7);
}

TEST(ArrayTest, stream) {
  TargetArray<int, 7> arr{1, 2, 3, 4, 5, 6, 7};
  std::stringstream stream;
  std::string ASSERTed("1 2 3 4 5 6 7");
  stream << arr;
  ASSERT_EQ(ASSERTed, stream.str());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
