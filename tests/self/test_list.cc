#include <list>
#include <random>
#include <sstream>

#include "gtest/gtest.h"
#include "sp/list.h"
#include "test_helpers.h"

template <typename T, typename Al = std::allocator<T>>
using TargetList = sp::list<T, Al>;

std::random_device ran_dev;
std::mt19937 gen(ran_dev());

#ifdef MAX_SIZE
std::uniform_int_distribution<dstd::size_t> uid(1, MAX_SIZE);
#else
std::uniform_int_distribution<int64_t> uid(1, 100);
#endif

#ifdef LOOP_COUNT
constexpr int loop = LOOP_COUNT;
#else
// constexpr int loop = 15;
#endif

TEST(ListTest, ctor_default) {
  TargetList<no_def> lst;
  ASSERT_EQ(lst.size(), 0);
  ASSERT_TRUE(lst.empty());
}

// TEST(ListTest, ctor_size_alloc_not_default) {
//   int64_t size = uid(gen);
//   state_allocator<safe> not_default;
//   TargetList<safe, state_allocator<safe>> lst(size, not_default);

//   ASSERT_EQ(lst.size(), size);
//   ASSERT_EQ(lst.front(), safe());
//   ASSERT_EQ(lst.back(), safe());
//   ASSERT_EQ(lst.front().birth, constructed::kDef);
//   ASSERT_EQ(lst.back().birth, constructed::kDef);
//   ASSERT_EQ(lst.get_allocator(), not_default);
// }

TEST(ListTest, ctor_size) {
  int64_t size = uid(gen);
  const TargetList<safe> lst(size, safe("lst"));
  ASSERT_EQ(lst.size(), size);
  ASSERT_FALSE(lst.empty());
  ASSERT_EQ(lst.front(), safe("lst"));
  ASSERT_EQ(lst.back(), safe("lst"));
}

TEST(ListTest, ctor_size_value) {
  int64_t size = uid(gen);

  const sp::list<safe> lst(size, safe("not defult"));
  ASSERT_EQ(lst.size(), size);
  ASSERT_FALSE(lst.empty());
  ASSERT_EQ(lst.front(), safe("not defult"));
  ASSERT_EQ(lst.back(), safe("not defult"));
}

TEST(ListTest, ctor_from_stl_list) {
  int64_t size = uid(gen);

  std::list<safe> from(size, safe("not default"));

  sp::list<safe> lst(from.begin(), from.end());

  ASSERT_EQ(lst.size(), from.size());
  ASSERT_EQ(lst.front(), from.front());
  ASSERT_EQ(lst.back(), from.back());
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

  sp::list<std::string> lst(list);
  ASSERT_EQ(lst.size(), list.size());
  ASSERT_EQ(lst.front(), "Karen");
}

TEST(ListTest, ctor_init_list_implicit) {
  sp::list<std::string> lst{
    "Karen",
    "Anastasia",
    "Alice",
    "Natalie",
    "Leyla",
    "Victoria"
  };

  ASSERT_EQ(lst.size(), 6);
  ASSERT_EQ(lst.front(), "Karen");
}

TEST(ListTest, ctor_copy) {
  int64_t size = uid(gen);
  sp::list<safe> lst1(size, safe("not default"));
  sp::list<safe> lst2(lst1);

  ASSERT_EQ(lst1.size(), lst2.size());
  ASSERT_EQ(lst1.front(), lst2.front());
  ASSERT_EQ(lst1.back(), lst2.front());
}

TEST(ListTest, ctor_move) {
  int64_t size = uid(gen);
  sp::list<safe> lst1(size, safe("not default"));
  sp::list<safe> lst2(std::move(lst1));

  ASSERT_EQ(lst2.size(), size);
  ASSERT_EQ(lst2.front(), safe("not default"));
  ASSERT_EQ(lst2.back(), safe("not default"));
}

// TEST(ListTest, ctor_move_alloc_not_default) {
//   int64_t size = uid(gen);
//   state_allocator<safe> not_default("not default");
//   sp::list<safe, state_allocator<safe>> lst1(size, safe("not default"),
//   state_allocator<safe>("default")); sp::list<safe, state_allocator<safe>>
//   lst2(std::move(lst1), not_default);

//   ASSERT_EQ(lst2.size(), size);
//   ASSERT_EQ(lst2.front(), safe("not default"));
//   ASSERT_EQ(lst2.back(), safe("not default"));
//   // ASSERT_EQ(lst2.get_allocator(), not_default);
// }

// TEST(ListTest, ctor_ass_copy) {
//   int64_t size = uid(gen);
//   sp::list<safe> lst1(size, safe("not default"));
//   sp::list<safe> lst2;

//   lst2 = lst1;

//   ASSERT_EQ(lst1.size(), lst2.size());
//   ASSERT_EQ(lst1.front(), lst2.front());
//   ASSERT_EQ(lst1.back(), lst2.front());
// }

// TEST(ListTest, ctor_ass_move) {
//   int64_t size = uid(gen);
//   sp::list<safe> lst1(size, safe("not default"));
//   sp::list<safe> lst2;

//   lst2 = std::move(lst1);

//   ASSERT_EQ(lst2.size(), size);
//   ASSERT_EQ(lst2.front(), safe("not default"));
//   ASSERT_EQ(lst2.back(), safe("not default"));
// }

// TEST(ListTest, comparison_1) {
//   int64_t size = uid(gen);
//   sp::list<safe> lst1(size, safe("equal"));
//   sp::list<safe> lst2(size, safe("equal"));

//   ASSERT_TRUE(lst1 == lst2);
//   ASSERT_FALSE(lst1 != lst2);
//   ASSERT_TRUE(lst2 == lst1);
//   ASSERT_FALSE(lst2 != lst1);
// }

// TEST(ListTest, comparison_2) {
//   sp::list<safe> lst1(uid(gen), safe("not equal"));
//   sp::list<safe> lst2(uid(gen), safe("equal"));

//   ASSERT_FALSE(lst1 == lst2);
//   ASSERT_TRUE(lst1 != lst2);
//   ASSERT_FALSE(lst2 == lst1);
//   ASSERT_TRUE(lst2 != lst1);
// }

// TEST(ListTest, comparison_3) {
//   sp::list<safe> lst1(uid(gen), safe("not equal"));
//   sp::list<safe> lst2;

//   ASSERT_FALSE(lst1 == lst2);
//   ASSERT_TRUE(lst1 != lst2);
//   ASSERT_FALSE(lst2 == lst1);
//   ASSERT_TRUE(lst2 != lst1);
// }

// TEST(ListTest, comparison_self) {
//   sp::list<safe> lst(uid(gen), safe("not equal"));

//   ASSERT_TRUE(lst == lst);
//   ASSERT_FALSE(lst != lst);
// }

// TEST(ListTest, comparison_empty) {
//   sp::list<safe> lst1;
//   sp::list<safe> lst2;

//   ASSERT_TRUE(lst1 == lst2);
//   ASSERT_FALSE(lst1 != lst2);
// }

// TEST(ListTest, clear) {
//   for (int i = 0; i < loop; i++) {
//     sp::list<safe> lst(uid(gen), safe("dirty"));

//     lst.clear();
//     ASSERT_EQ(lst.size(), 0);
//   }
// }

// TEST(ListTest, insert_movable) {
//   for (int i = 0; i < loop; i++) {
//     sp::list<not_safe> lst(uid(gen), not_safe("default"));
//     std::uniform_int_distribution<int64_t> uid_lst(0, lst.size());
//     int64_t pos = uid_lst(gen);
//     auto insert_pos = lst.begin();
//     for (int64_t i = 0; i < pos; ++i, ++insert_pos) {};
//     insert_pos = lst.insert(insert_pos, not_safe("inserted"));
//     ASSERT_EQ(*insert_pos, not_safe("inserted"));
//   }
// }

// TEST(ListTest, insert_empty_movable) {
//   sp::list<not_safe> lst;
//   auto insert_pos = lst.insert(lst.begin(), not_safe("inserted"));
//   ASSERT_EQ(insert_pos, lst.begin());
// }

// TEST(ListTest, insert_begin_movable) {
//   sp::list<not_safe> lst(uid(gen), not_safe("default"));
//   auto insert_pos = lst.insert(lst.begin(), not_safe("inserted"));
//   ASSERT_EQ(*insert_pos, not_safe("inserted"));
//   ASSERT_EQ(*lst.begin(), not_safe("inserted"));
//   ASSERT_EQ(lst.begin(), insert_pos);
// }

// TEST(ListTest, stream) {
//   sp::list<not_safe> lst{
//       not_safe("Aileen"), not_safe("Anna"),  not_safe("Louie"),
//       not_safe("Noel"),   not_safe("Grace"),
//   };
//   std::stringstream stream;
//   std::string ASSERTed("Aileen Anna Louie Noel Grace");
//   stream << lst;
//   ASSERT_EQ(ASSERTed, stream.str());
// }
