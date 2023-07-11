#include <gtest/gtest.h>

#include <array>

#include <dlwhi/array.h>

TEST(ArrayTest, test0) {
  dlwhi::array<int, 7> s21arr{1, 2, 3, 4, 5, 6, 7};
  std::array<int, 7> arr{1, 2, 3, 4, 5, 6, 7};

  EXPECT_EQ(s21arr.at(1), arr.at(1));
  EXPECT_EQ(s21arr.at(4), arr.at(4));
  EXPECT_EQ(s21arr.front(), arr.front());
  EXPECT_EQ(s21arr.back(), arr.back());

  EXPECT_EQ(*s21arr.begin(), *arr.begin());
  EXPECT_EQ(*(s21arr.end() - 1), *(arr.end() - 1));
  EXPECT_EQ(*s21arr.rbegin(), *arr.rbegin());
  EXPECT_EQ(*(s21arr.rend() - 1), *(arr.rend() - 1));

  s21arr.fill(4);
  arr.fill(4);
  auto s21it = s21arr.begin();
  auto it = arr.begin();
  for (; s21it != s21arr.end(); it++, s21it++) EXPECT_EQ(*s21it, *it);

  dlwhi::array<int, 7> other{1, 2, 3, 4, 5, 6, 7};
  other.swap(s21arr);
  s21it = other.begin();
  for (it = arr.begin(); s21it != other.end(); it++, s21it++)
    EXPECT_EQ(*s21it, *it);

  for (size_t i = 0; i < other.size(); i++) other[i] = i + 1;

  EXPECT_EQ(other, s21arr);
}

TEST(ArrayTest, test1) {
  const dlwhi::array<int, 2> s21arr_const{1, 2};
  std::array<int, 2> arr{1, 2};

  EXPECT_EQ(s21arr_const.at(1), arr.at(1));
  EXPECT_EQ(s21arr_const.at(0), arr.at(0));
  EXPECT_EQ(s21arr_const.front(), arr.front());
  EXPECT_EQ(s21arr_const.back(), arr.back());

  EXPECT_EQ(*s21arr_const.begin(), *arr.begin());
  EXPECT_EQ(*(s21arr_const.end() - 1), *(arr.end() - 1));
  EXPECT_EQ(*s21arr_const.rbegin(), *arr.rbegin());
  EXPECT_EQ(*(s21arr_const.rend() - 1), *(arr.rend() - 1));

  constexpr size_t size_arr = arr.size();
  constexpr size_t size_s21arr = s21arr_const.size();
  EXPECT_EQ(size_arr, size_s21arr);

  constexpr bool empty = s21arr_const.empty();
  EXPECT_FALSE(empty);

  constexpr size_t max_size_s21arr = s21arr_const.max_size();
  constexpr size_t max_size_arr = arr.max_size();
  EXPECT_EQ(max_size_s21arr, max_size_arr);
}

TEST(ArrayTest, scenartio_stream) {
  dlwhi::array<int, 7> arr{1, 2, 3, 4, 5, 6, 7};
  std::stringstream stream;
  std::string expected("1 2 3 4 5 6 7");
  stream << arr;
  EXPECT_EQ(expected, stream.str());
}
