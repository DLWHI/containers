#include <gtest/gtest.h>

#include <sstream>
#include <list>
#include <random>

#include <dlwhi/list.h>
#include "test_helpers.h"

std::random_device ran_dev;
std::mt19937 gen(ran_dev());

#ifdef MAX_SIZE
std::uniform_int_distribution<dlwhi::size_t> uid(1, MAX_SIZE);
#else
std::uniform_int_distribution<dlwhi::size_t> uid(1, 100);
#endif

#ifdef LOOP_COUNT
constexpr int loop = LOOP_COUNT;
#else
constexpr int loop = 15;
#endif

TEST(ListTest, ctor_default) {
  dlwhi::list<dummyNoDef> lst;
  EXPECT_EQ(lst.size(), 0);
  EXPECT_TRUE(lst.empty());
}

TEST(ListTest, ctor_size_alloc_not_default) {
  dlwhi::size_t size = uid(gen);
  std::allocator<dummyCpy> not_default;
  dlwhi::list<dummyCpy> lst(size, not_default);

  EXPECT_EQ(lst.size(), size);
  EXPECT_EQ(lst.front(), dummyCpy());
  EXPECT_EQ(lst.back(), dummyCpy());
}

TEST(ListTest, ctor_size) {
  dlwhi::size_t size = uid(gen);
  std::allocator<dummyCpy> not_default;
  const dlwhi::list<dummyCpy> lst(size, not_default);
  EXPECT_EQ(lst.size(), size);
  EXPECT_FALSE(lst.empty());
  EXPECT_EQ(lst.front(), dummyCpy());
  EXPECT_EQ(lst.back(), dummyCpy());
}

TEST(ListTest, ctor_size_value) {
  dlwhi::size_t size = uid(gen);
  std::allocator<dummyCpy> not_default;
  const dlwhi::list<dummyCpy> lst(size, dummyCpy("not defult"), not_default);
  EXPECT_EQ(lst.size(), size);
  EXPECT_FALSE(lst.empty());
  EXPECT_EQ(lst.front(), dummyCpy("not defult"));
  EXPECT_EQ(lst.back(), dummyCpy("not defult"));
}

TEST(ListTest, ctor_from_stl_list) {
  dlwhi::size_t size = uid(gen);
  std::allocator<dummyCpy> not_default;
  std::list<dummyCpy> from(size, dummyCpy("not default"));

  dlwhi::list<dummyCpy> lst(from.begin(), from.end(), not_default);

  EXPECT_EQ(lst.size(), from.size());
  EXPECT_EQ(lst.front(), from.front());
  EXPECT_EQ(lst.back(), from.back());
}

TEST(ListTest, ctor_init_list) {
  std::initializer_list<std::string> list{
    "Karen",
    "Anastasia",
    "Alice",
    "Natalie",
    "Leyla",
    "Victoria"
  };

  std::allocator<std::string> not_default;
  dlwhi::list<std::string> lst(list, not_default);
  EXPECT_EQ(lst.size(), list.size());
  EXPECT_EQ(lst.front(), "Karen");
}

TEST(ListTest, ctor_init_list_implicit) {
  dlwhi::list<std::string> lst{
    "Karen",
    "Anastasia",
    "Alice",
    "Natalie",
    "Leyla",
    "Victoria"
  };

  EXPECT_EQ(lst.size(), 6);
  EXPECT_EQ(lst.front(), "Karen");
}

TEST(ListTest, ctor_copy) {
  dlwhi::size_t size = uid(gen);
  dlwhi::list<dummyCpy> lst1(size, dummyCpy("not default"));
  dlwhi::list<dummyCpy> lst2(lst1);

  EXPECT_EQ(lst1.size(), lst2.size());
  EXPECT_EQ(lst1.front(), lst2.front());
  EXPECT_EQ(lst1.back(), lst2.front());
}

TEST(ListTest, ctor_move) {
  dlwhi::size_t size = uid(gen);
  dlwhi::list<dummyCpy> lst1(size, dummyCpy("not default"));
  dlwhi::list<dummyCpy> lst2(std::move(lst1));

  EXPECT_EQ(lst2.size(), size);
  EXPECT_EQ(lst2.front(), dummyCpy("not default"));
  EXPECT_EQ(lst2.back(), dummyCpy("not default"));
}

TEST(ListTest, ctor_move_alloc_not_default) {
  dlwhi::size_t size = uid(gen);
  state_allocator<dummyCpy> not_default("not default");
  dlwhi::list<dummyCpy, state_allocator<dummyCpy>> lst1(size, dummyCpy("not default"), state_allocator<dummyCpy>("default"));
  dlwhi::list<dummyCpy, state_allocator<dummyCpy>> lst2(std::move(lst1), not_default);

  EXPECT_EQ(lst2.size(), size);
  EXPECT_EQ(lst2.front(), dummyCpy("not default"));
  EXPECT_EQ(lst2.back(), dummyCpy("not default"));
  // EXPECT_EQ(lst2.get_allocator(), not_default);
}

TEST(ListTest, ctor_ass_copy) {
  dlwhi::size_t size = uid(gen);
  dlwhi::list<dummyCpy> lst1(size, dummyCpy("not default"));
  dlwhi::list<dummyCpy> lst2;

  lst2 = lst1;

  EXPECT_EQ(lst1.size(), lst2.size());
  EXPECT_EQ(lst1.front(), lst2.front());
  EXPECT_EQ(lst1.back(), lst2.front());
}

TEST(ListTest, ctor_ass_move) {
  dlwhi::size_t size = uid(gen);
  dlwhi::list<dummyCpy> lst1(size, dummyCpy("not default"));
  dlwhi::list<dummyCpy> lst2;

  lst2 = std::move(lst1);

  EXPECT_EQ(lst2.size(), size);
  EXPECT_EQ(lst2.front(), dummyCpy("not default"));
  EXPECT_EQ(lst2.back(), dummyCpy("not default"));
}

TEST(ListTest, comparison_1) {
  dlwhi::size_t size = uid(gen);
  dlwhi::list<dummyCpy> lst1(size, dummyCpy("equal"));
  dlwhi::list<dummyCpy> lst2(size, dummyCpy("equal"));

  EXPECT_TRUE(lst1 == lst2);
  EXPECT_FALSE(lst1 != lst2);
  EXPECT_TRUE(lst2 == lst1);
  EXPECT_FALSE(lst2 != lst1);
}

TEST(ListTest, comparison_2) {
  dlwhi::list<dummyCpy> lst1(uid(gen), dummyCpy("not equal"));
  dlwhi::list<dummyCpy> lst2(uid(gen), dummyCpy("equal"));

  EXPECT_FALSE(lst1 == lst2);
  EXPECT_TRUE(lst1 != lst2);
  EXPECT_FALSE(lst2 == lst1);
  EXPECT_TRUE(lst2 != lst1);
}

TEST(ListTest, comparison_3) {
  dlwhi::list<dummyCpy> lst1(uid(gen), dummyCpy("not equal"));
  dlwhi::list<dummyCpy> lst2;

  EXPECT_FALSE(lst1 == lst2);
  EXPECT_TRUE(lst1 != lst2);
  EXPECT_FALSE(lst2 == lst1);
  EXPECT_TRUE(lst2 != lst1);
}

TEST(ListTest, comparison_self) {
  dlwhi::list<dummyCpy> lst(uid(gen), dummyCpy("not equal"));

  EXPECT_TRUE(lst == lst);
  EXPECT_FALSE(lst != lst);
}

TEST(ListTest, comparison_empty) {
  dlwhi::list<dummyCpy> lst1;
  dlwhi::list<dummyCpy> lst2;

  EXPECT_TRUE(lst1 == lst2);
  EXPECT_FALSE(lst1 != lst2);
}

TEST(ListTest, clear) {
  for (int i = 0; i < loop; i++) {
    dlwhi::list<dummyCpy> lst(uid(gen), dummyCpy("dirty"));

    lst.clear();
    EXPECT_EQ(lst.size(), 0);
  }
}

TEST(ListTest, insert_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::list<dummyMv> lst(uid(gen), dummyMv("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_lst(0, lst.size());
    dlwhi::size_t pos = uid_lst(gen);
    auto insert_pos = lst.begin();
    for (dlwhi::size_t i = 0; i < pos; ++i, ++insert_pos) {};
    insert_pos = lst.insert(insert_pos, dummyMv("inserted"));
    EXPECT_EQ(*insert_pos, dummyMv("inserted"));
  }
}

TEST(ListTest, insert_empty_movable) {
  dlwhi::list<dummyMv> lst;
  auto insert_pos = lst.insert(lst.begin(), dummyMv("inserted"));
  EXPECT_EQ(insert_pos, lst.begin());
}

TEST(ListTest, insert_begin_movable) {
  dlwhi::list<dummyMv> lst(uid(gen), dummyMv("default"));
  auto insert_pos = lst.insert(lst.begin(), dummyMv("inserted"));
  EXPECT_EQ(*insert_pos, dummyMv("inserted"));
  EXPECT_EQ(*lst.begin(), dummyMv("inserted"));
  EXPECT_EQ(lst.begin(), insert_pos);
}

TEST(ListTest, stream) {
  dlwhi::list<dummyMv> lst{
      dummyMv("Aileen"), dummyMv("Anna"),  dummyMv("Louie"),
      dummyMv("Noel"),   dummyMv("Grace"),
  };
  std::stringstream stream;
  std::string expected("Aileen Anna Louie Noel Grace");
  stream << lst;
  EXPECT_EQ(expected, stream.str());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}