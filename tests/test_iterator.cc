#include <gtest/gtest.h>

#include <dlwhi/iterator.h>
#include <dlwhi/vector.h>

struct IntWrapper {
  int data;
};

TEST(ItertorTest, test0) {
  s21::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  s21::vector<int>::v_iterator it = vec.begin();
  s21::vector<int>::v_iterator it_cpy_ass = it;
  s21::vector<int>::v_iterator it_mv(std::move(it));
  EXPECT_EQ(it_cpy_ass, it_mv);

  it_mv++;
  EXPECT_GT(it_mv, it_cpy_ass);
  EXPECT_GE(it_mv, it_cpy_ass);
  EXPECT_LT(it_cpy_ass, it_mv);
  EXPECT_LE(it_cpy_ass, it_mv);
  EXPECT_EQ(it_mv - it_cpy_ass, 1);

  it_mv--;
  --(++it_cpy_ass);
  EXPECT_EQ(it_cpy_ass, it_mv);
}

TEST(ItertorTest, test1) {
  s21::vector<IntWrapper> vec(3);
  vec.front().data = 1;
  vec.back().data = 3;

  s21::vector<IntWrapper>::v_iterator it = --vec.end();
  EXPECT_EQ(it->data, 3);
}

TEST(RIteratorTest, test0) {
  s21::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  s21::vector<int>::v_reverse_iterator it = vec.rbegin();
  auto it_cpy_ass = it;
  auto it_mv(std::move(it));
  EXPECT_EQ(it_cpy_ass, it_mv);

  it_mv--;
  EXPECT_LT(it_mv, it_cpy_ass);
  EXPECT_LE(it_mv, it_cpy_ass);
  EXPECT_GT(it_cpy_ass, it_mv);
  EXPECT_GE(it_cpy_ass, it_mv);
  EXPECT_EQ(it_cpy_ass - it_mv, 1);

  it_mv = it_mv - 1;

  ++ ++it_mv;
  EXPECT_EQ(it_cpy_ass, it_mv);

  it_cpy_ass = it_cpy_ass + 1;
  EXPECT_EQ(9, *it_cpy_ass);
  --it_cpy_ass;
  EXPECT_EQ(it_cpy_ass, it_mv);

  auto it_const =
      static_cast<s21::vector<int>::v_creverse_iterator>(vec.crbegin());
  EXPECT_EQ(*it_const, *vec.crbegin());
  EXPECT_EQ(it_const.base(), vec.data() + vec.size() - 1);
}
