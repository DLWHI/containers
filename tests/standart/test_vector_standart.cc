
#include <chrono>
#include <iostream>
#include <list>
#include <random>
#include <sstream>
#include <vector>

#include "gtest/gtest.h"
#include "sp/pool_allocator.h"
#include "sp/vector.h"
#include "test_helpers.h"

template <typename T, typename Al = std::allocator<T>>
using TargetVector = std::vector<T, Al>;

std::random_device ran_dev;
std::mt19937 gen(ran_dev());

#ifdef MAX_SIZE
std::uniform_int_distribution<int> uid(1, MAX_SIZE);
#else
std::uniform_int_distribution<int> uid(1, 100);
#endif

#ifdef LOOP_COUNT
constexpr int loop = LOOP_COUNT;
#else
// constexpr int loop = 15;
#endif

//==============================================================================
// ctors
TEST(VectorTest, ctor_default) {
  using subject = tclass<traits::NoTraits>;
  const TargetVector<subject> vec;
  ASSERT_EQ(vec.size(), 0);
  ASSERT_EQ(vec.capacity(), 0);
  ASSERT_EQ(vec.data(), nullptr);
  ASSERT_THROW(vec.at(uid(gen)), std::out_of_range);
}

TEST(VectorTest, ctor_size_alloc_not_default) {
  using subject = tclass<traits::DefaultInsertable>;
  int size = uid(gen);
  sp::pool_allocator<subject> not_default(size * 3 / 2);
  const TargetVector<subject, sp::pool_allocator<subject>> vec(size,
                                                               not_default);

  ASSERT_EQ(static_cast<int>(vec.size()), size);
  ASSERT_TRUE(vec.get_allocator() == not_default);
  ASSERT_EQ(not_default.allocd(), size);
  ASSERT_EQ(vec.front(), subject());
  ASSERT_EQ(vec.back(), subject());
}

TEST(VectorTest, ctor_size_bad_alloc) {
  using subject = tclass<traits::DefaultInsertable>;
  int size = uid(gen);
  sp::pool_allocator<subject> not_default(size);

  try {
    const TargetVector<subject, sp::pool_allocator<subject>> vec(size * 2,
                                                                 not_default);
    FAIL();
  } catch (...) {
  }
}

TEST(VectorTest, ctor_size) {
  using subject = tclass<traits::DefaultInsertable>;
  int size = uid(gen);
  const TargetVector<subject> vec(size);

  ASSERT_EQ(static_cast<int>(vec.size()), size);
  ASSERT_EQ(vec.front(), subject());
  ASSERT_EQ(vec.back(), subject());
}

TEST(VectorTest, ctor_size_zero) {
  using subject = tclass<traits::DefaultInsertable>;
  const TargetVector<subject> vec(0);

  ASSERT_EQ(static_cast<int>(vec.size()), 0);
  ASSERT_THROW(vec.at(uid(gen)), std::out_of_range);
}

TEST(VectorTest, ctor_size_value) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen);
  const TargetVector<subject> vec(size, subject("not default"));

  ASSERT_EQ(static_cast<int>(vec.size()), size);
  ASSERT_NE(vec.data(), nullptr);
  ASSERT_EQ(vec.front(), subject("not default"));
  ASSERT_EQ(vec.back(), subject("not default"));
}

TEST(VectorTest, ctor_size_value_zero) {
  using subject = tclass<traits::CopyInsertable>;
  const TargetVector<subject> vec(0, subject("not default"));

  ASSERT_EQ(static_cast<int>(vec.size()), 0);
}

TEST(VectorTest, ctor_size_value_throwing) {
  throwing::count = 0;
  int size = uid(gen) + 5;
  std::allocator<throwing> not_default;
  ASSERT_ANY_THROW(
      TargetVector<throwing> vec(size, throwing("not default"), not_default));
}

TEST(VectorTest, ctor_from_stl_vector) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen);
  std::vector<subject> from(size, subject("not default"));

  const TargetVector<subject> vec(from.begin(), from.end());
  ASSERT_EQ(vec.size(), from.size());
  ASSERT_NE(vec.data(), nullptr);
  ASSERT_EQ(vec.front(), from.front());
  ASSERT_EQ(vec.back(), from.back());
}

TEST(VectorTest, ctor_from_stl_vector_strings) {
  using subject = tclass<traits::NoTraits>;
  int size = uid(gen);
  std::vector<std::string> from(size, "not default");

  const TargetVector<subject> vec(from.begin(), from.end());
  ASSERT_EQ(vec.size(), from.size());
  ASSERT_NE(vec.data(), nullptr);
  ASSERT_EQ(vec.front(), subject(from.front()));
  ASSERT_EQ(vec.back(), subject(from.back()));
}

TEST(VectorTest, ctor_from_stl_list) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen);
  std::list<subject> from(size, subject("not default"));

  const TargetVector<subject> vec(from.begin(), from.end());

  ASSERT_EQ(vec.size(), from.size());
  ASSERT_NE(vec.data(), nullptr);
  ASSERT_EQ(vec.front(), from.front());
  ASSERT_EQ(vec.back(), from.back());
}

TEST(VectorTest, ctor_range_empty) {
  using subject = tclass<traits::CopyInsertable>;
  const std::allocator<subject> not_default;
  const subject range[] = {subject("first"), subject("second"),
                           subject("last")};
  const TargetVector<subject> vec(range, range, not_default);
  ASSERT_EQ(vec.size(), 0);
}

TEST(VectorTest, ctor_range_throwing) {
  throwing::count = 0;
  const std::allocator<throwing> not_default;
  const throwing range[] = {throwing("first"), throwing("second"),
                            throwing("last"), throwing("last"),
                            throwing("last")};
  ASSERT_ANY_THROW(const TargetVector<throwing> vec(
      std::begin(range), std::end(range), not_default));
}

TEST(VectorTest, ctor_init_list) {
  using subject = tclass<traits::CopyInsertable>;
  std::initializer_list<subject> list{
      subject("Karen"),   subject("Anastasia"), subject("Alice"),
      subject("Natalie"), subject("Leyla"),     subject("Victoria")};

  const TargetVector<subject> vec(list);
  ASSERT_EQ(vec.size(), list.size());
  ASSERT_NE(vec.data(), nullptr);
  ASSERT_EQ(vec.front(), subject("Karen"));
}

TEST(VectorTest, ctor_init_list_implicit) {
  const TargetVector<std::string> vec{"Karen",   "Anastasia", "Alice",
                                      "Natalie", "Leyla",     "Victoria"};

  ASSERT_EQ(vec.size(), 6);
  ASSERT_EQ(vec.front(), "Karen");
}

TEST(VectorTest, ctor_init_list_empty) {
  const TargetVector<std::string> vec{};

  ASSERT_EQ(vec.size(), 0);
}

TEST(VectorTest, ctor_init_list_throwing) {
  throwing::count = 0;
  ASSERT_ANY_THROW(TargetVector<throwing> vec(
      {throwing(), throwing(), throwing(), throwing(), throwing()}));
}

TEST(VectorTest, ctor_copy) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen);
  const TargetVector<subject> vec1(size, subject("not default"));
  const TargetVector<subject> vec2(vec1);

  ASSERT_EQ(vec1.size(), vec2.size());
  ASSERT_EQ(vec1.front(), vec2.front());
  ASSERT_EQ(vec1.back(), vec2.front());
  ASSERT_EQ(vec2.front().birth, constructed::kCopy);
  ASSERT_EQ(vec2.back().birth, constructed::kCopy);
}

TEST(VectorTest, ctor_copy_alloc) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen);
  sp::pool_allocator<subject> al(size);
  const TargetVector<subject, sp::pool_allocator<subject>> vec1(
      size / 2, subject("not default"), al);
  const TargetVector<subject, sp::pool_allocator<subject>> vec2(vec1);

  ASSERT_EQ(vec1.size(), vec2.size());
  ASSERT_EQ(vec1.capacity(), vec2.capacity());
  ASSERT_NE(vec1.data(), vec2.data());
  ASSERT_EQ(vec1.front(), vec2.front());
  ASSERT_EQ(vec1.back(), vec2.front());
  ASSERT_EQ(vec1.get_allocator(), vec2.get_allocator());
  ASSERT_EQ(vec2.front().birth, constructed::kCopy);
  ASSERT_EQ(vec2.back().birth, constructed::kCopy);
}

TEST(VectorTest, ctor_copy_size_zero) {
  using subject = tclass<traits::CopyInsertable>;
  const TargetVector<subject> vec1(0, subject("str"));
  const TargetVector<subject> vec2(vec1);

  ASSERT_EQ(vec1.size(), vec2.size());
  ASSERT_EQ(vec1.data(), vec2.data());
}

TEST(VectorTest, ctor_copy_alloc_always_equal) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen);
  std::allocator<subject> not_default;
  TargetVector<subject> vec1(size, subject("str"));
  const TargetVector<subject> vec2(vec1, not_default);

  ASSERT_EQ(vec1.size(), vec2.size());
  ASSERT_NE(vec1.data(), vec2.data());
  ASSERT_EQ(vec1.front(), vec2.front());
  ASSERT_EQ(vec1.back(), vec2.front());
  ASSERT_EQ(vec2.front().birth, constructed::kCopy);
  ASSERT_EQ(vec2.back().birth, constructed::kCopy);
}

TEST(VectorTest, ctor_copy_alloc_not_equal) {
  using subject = tclass<traits::CopyInsertable>;
  int size = uid(gen) + 10;
  sp::pool_allocator<subject> al1(size);
  sp::pool_allocator<subject> al2(size * 2);
  TargetVector<subject, sp::pool_allocator<subject>> vec1(size, subject("s"), al1);

  const TargetVector<subject, sp::pool_allocator<subject>> vec2(vec1, al2);
  ASSERT_EQ(vec1.size(), vec2.size());
  ASSERT_EQ(vec1.capacity(), vec2.capacity());
  ASSERT_NE(vec1.data(), vec2.data());
  ASSERT_EQ(vec1.front(), vec2.front());
  ASSERT_EQ(vec1.back(), vec2.front());
  ASSERT_TRUE(vec1.get_allocator() != vec2.get_allocator());
  ASSERT_TRUE(vec1.get_allocator() == al1);
  ASSERT_TRUE(vec2.get_allocator() == al2);
  ASSERT_TRUE(al2.allocd() == al1.allocd());
  ASSERT_EQ(vec2.front().birth, constructed::kCopy);
  ASSERT_EQ(vec2.back().birth, constructed::kCopy);
}

TEST(VectorTest, ctor_copy_throwing_alloc_not_default) {
  throwing::count = 0;
  int size = uid(gen) + 5;
  sp::pool_allocator<throwing> al1(size);
  sp::pool_allocator<throwing> al2(size);
  TargetVector<throwing, sp::pool_allocator<throwing>> vec1(size, al1);
  try {
    TargetVector<throwing, sp::pool_allocator<throwing>> vec2(vec1, al2);
    FAIL();
  } catch (...) {
  }

  ASSERT_EQ(vec1.size(), size);
  ASSERT_NE(vec1.data(), nullptr);
  for (auto i = 0; i < size; ++i) {
    ASSERT_NO_THROW(vec1.at(i));
  }
}

TEST(VectorTest, ctor_copy_throwing) {
  throwing::count = 0;
  int size = uid(gen);
  const TargetVector<throwing> vec1(size + 5);
  ASSERT_ANY_THROW(TargetVector<throwing> vec2(vec1));
}

TEST(VectorTest, ctor_move) {
  using subject = tclass<traits::DefaultInsertable>;
  int size = uid(gen);
  TargetVector<subject> vec1(size);
  int cap = vec1.capacity();
  subject *ptr = vec1.data();
  const TargetVector<subject> vec2(std::move(vec1));

  ASSERT_EQ(vec2.size(), size);
  ASSERT_EQ(vec2.capacity(), cap);
  ASSERT_EQ(vec2.data(), ptr);
  ASSERT_EQ(vec2.front(), subject());
  ASSERT_EQ(vec2.back(), subject());
  ASSERT_EQ(vec2.front().birth, constructed::kDef);
  ASSERT_EQ(vec2.back().birth, constructed::kDef);
}

TEST(VectorTest, ctor_move_throwing) {
  int size = uid(gen);
  TargetVector<throwing> vec1(size);

  ASSERT_NO_THROW(const TargetVector<throwing> vec2(std::move(vec1)););
}

TEST(VectorTest, ctor_move_safe_alloc_always_equal) {
  using subject = tclass<traits::MoveInsertable | traits::DefaultInsertable>;
  int size = uid(gen);
  std::allocator<subject> not_default;
  TargetVector<subject> vec1(size);
  int cap = vec1.capacity();
  subject *ptr = vec1.data();
  const TargetVector<subject> vec2(std::move(vec1), not_default);

  ASSERT_EQ(vec2.size(), size);
  ASSERT_EQ(vec2.capacity(), cap);
  ASSERT_EQ(vec2.data(), ptr);
  ASSERT_EQ(vec2.front().birth, constructed::kDef);
  ASSERT_EQ(vec2.back().birth, constructed::kDef);
}

// TEST(VectorTest, ctor_move_safe_alloc_equal) {
//   int size = uid(gen);
//   sp::pool_allocator<safe> al(size * 2);
//   TargetVector<safe, sp::pool_allocator<safe>> vec1(size, al);
//   int cap = vec1.capacity();
//   const TargetVector<safe, sp::pool_allocator<safe>> vec2(std::move(vec1),
//   al);

//   ASSERT_EQ(vec2.size(), size);
//   ASSERT_EQ(vec2.capacity(), cap);
//   ASSERT_NE(vec1.data(), vec2.data());
//   ASSERT_EQ(vec2.front(), safe());
//   ASSERT_EQ(vec2.back(), safe());
//   ASSERT_TRUE(vec1.get_allocator() == vec2.get_allocator());
//   ASSERT_TRUE(vec2.get_allocator() == al);
//   ASSERT_EQ(al.leftover(), size);
//   ASSERT_EQ(vec2.front().birth, constructed::kDef);
//   ASSERT_EQ(vec2.back().birth, constructed::kDef);
// }

// TEST(VectorTest, ctor_move_safe_alloc) {
//   int size = uid(gen);
//   sp::pool_allocator<safe> al1(size);
//   sp::pool_allocator<safe> al2(size);
//   TargetVector<safe, sp::pool_allocator<safe>> vec1(size, safe("not
//   default"),
//                                                     al1);
//   int cap = vec1.capacity();
//   const TargetVector<safe, sp::pool_allocator<safe>> vec2(std::move(vec1),
//   al2); ASSERT_EQ(vec2.size(), size); ASSERT_EQ(vec2.capacity(), cap);
//   ASSERT_NE(vec1.data(), vec2.data());
//   ASSERT_EQ(vec2.front(), safe("not default"));
//   ASSERT_EQ(vec2.back(), safe("not default"));
//   ASSERT_TRUE(vec2.get_allocator() == al2);
//   ASSERT_TRUE(vec1.get_allocator() != vec2.get_allocator());
//   ASSERT_TRUE(vec1.get_allocator() == al1);
//   ASSERT_EQ(al1.leftover(), 0);
//   ASSERT_EQ(al2.leftover(), 0);
//   ASSERT_EQ(vec2.front().birth, constructed::kMove);
//   ASSERT_EQ(vec2.back().birth, constructed::kMove);
// }

// TEST(VectorTest, ctor_move_not_safe_alloc) {
//   int size = uid(gen);
//   sp::pool_allocator<not_safe> al1(size);
//   sp::pool_allocator<not_safe> al2(size);
//   TargetVector<not_safe, sp::pool_allocator<not_safe>> vec1(
//       size, not_safe("not default"), al1);
//   int cap = vec1.capacity();
//   const TargetVector<not_safe, sp::pool_allocator<not_safe>> vec2(
//       std::move(vec1), al2);

//   ASSERT_EQ(vec2.size(), size);
//   ASSERT_EQ(vec2.capacity(), cap);
//   ASSERT_NE(vec1.data(), vec2.data());
//   ASSERT_EQ(vec2.front(), not_safe("not default"));
//   ASSERT_EQ(vec2.back(), not_safe("not default"));
//   ASSERT_TRUE(vec2.get_allocator() == al2);
//   ASSERT_TRUE(vec1.get_allocator() != vec2.get_allocator());
//   ASSERT_EQ(al1.leftover(), 0);
//   ASSERT_EQ(al2.leftover(), 0);
//   ASSERT_EQ(vec2.front().birth, constructed::kCopy);
//   ASSERT_EQ(vec2.back().birth, constructed::kCopy);
// }

// TEST(VectorTest, ctor_move_throwing_alloc_not_default) {
//   throwing::count = 0;
//   int size = uid(gen) + 5;
//   sp::pool_allocator<throwing> al1(size);
//   sp::pool_allocator<throwing> al2(size);
//   TargetVector<throwing, sp::pool_allocator<throwing>> vec1(size, al1);
//   try {
//     TargetVector<throwing, sp::pool_allocator<throwing>>
//     vec2(std::move(vec1),
//                                                               al2);
//     FAIL();
//   } catch (...) {
//   }

//   ASSERT_EQ(vec1.size(), size);
//   ASSERT_NE(vec1.data(), nullptr);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec1.at(i));
//   }
// }

// TEST(VectorTest, assignment_copy) {
//   int size = uid(gen);
//   const TargetVector<safe> vec1(size, safe("not default"));
//   TargetVector<safe> vec2;

//   vec2 = vec1;

//   ASSERT_EQ(vec1.size(), vec2.size());
//   ASSERT_EQ(vec1.capacity(), vec2.capacity());
//   ASSERT_NE(vec2.data(), nullptr);
//   ASSERT_EQ(vec1.front(), vec2.front());
//   ASSERT_EQ(vec1.back(), vec2.front());
//   ASSERT_EQ(vec2.front().birth, constructed::kCopy);
//   ASSERT_EQ(vec2.back().birth, constructed::kCopy);
// }

// TEST(VectorTest, assignment_copy_throwing) {
//   throwing::count = 0;
//   int size = uid(gen) + 5;
//   const TargetVector<throwing> vec1(size);
//   TargetVector<throwing> vec2(size + 7);

//   ASSERT_ANY_THROW(vec2 = vec1);

//   ASSERT_EQ(vec1.size() + 7, vec2.size());
//   ASSERT_EQ(vec1.capacity() + 7, vec2.capacity());
//   ASSERT_NE(vec2.data(), nullptr);
//   ASSERT_NE(vec1.data(), nullptr);
//   ASSERT_EQ(vec1.front(), vec2.front());
//   ASSERT_EQ(vec1.back(), vec2.front());
//   for (int i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec1.at(i));
//     ASSERT_NO_THROW(vec2.at(i));
//   }
// }

// TEST(VectorTest, assignment_move) {
//   int size = uid(gen);
//   TargetVector<not_safe> vec1(size);
//   int cap = vec1.capacity();
//   not_safe *ptr = vec1.data();
//   TargetVector<not_safe> vec2;

//   vec2 = std::move(vec1);

//   ASSERT_EQ(vec2.size(), size);
//   ASSERT_EQ(vec2.capacity(), cap);
//   ASSERT_NE(vec2.data(), ptr);
//   ASSERT_EQ(vec2.front(), not_safe());
//   ASSERT_EQ(vec2.back(), not_safe());
// }

// TEST(VectorTest, assignment_move_with_alloc) {
//   int size = uid(gen);
//   sp::pool_allocator<safe> al1(size);
//   sp::pool_allocator<safe> al2(size);
//   TargetVector<safe, sp::pool_allocator<safe>> vec1(size, al1);
//   int cap = vec1.capacity();
//   TargetVector<safe, sp::pool_allocator<safe>> vec2(al2);

//   vec2 = std::move(vec1);

//   ASSERT_EQ(vec2.size(), size);
//   ASSERT_EQ(vec2.capacity(), cap);
//   ASSERT_NE(vec2.data(), nullptr);
//   ASSERT_NE(vec2.get_allocator(), vec1.get_allocator());
//   ASSERT_EQ(vec2.front(), safe());
//   ASSERT_EQ(vec2.back(), safe());
// }

// TEST(VectorTest, assignment_move_with_alloc_no_realloc) {
//   sp::pool_allocator<safe> al1(10);
//   sp::pool_allocator<safe> al2(33);
//   TargetVector<safe, sp::pool_allocator<safe>> vec1(10, al1);

//   TargetVector<safe, sp::pool_allocator<safe>> vec2(33, al2);
//   safe *ptr = vec2.data();

//   vec2 = std::move(vec1);

//   ASSERT_EQ(vec2.size(), 10);
//   ASSERT_EQ(vec1.capacity(), 10);
//   ASSERT_EQ(vec2.capacity(), 33);
//   ASSERT_EQ(vec2.data(), ptr);
//   ASSERT_NE(vec2.get_allocator(), vec1.get_allocator());
//   ASSERT_EQ(vec2.front(), safe());
//   ASSERT_EQ(vec2.back(), safe());
// }

// TEST(VectorTest, swap) {
//   TargetVector<not_safe> vec1(10, not_safe("one"));

//   TargetVector<not_safe> vec2(33, not_safe("two"));

//   TargetVector<not_safe> exp1(vec2);
//   TargetVector<not_safe> exp2(vec1);

//   not_safe *ptr1 = vec1.data();
//   not_safe *ptr2 = vec2.data();

//   vec1.swap(vec2);

//   ASSERT_EQ(vec1, exp1);
//   ASSERT_EQ(vec2, exp2);
//   ASSERT_EQ(vec1.data(), ptr2);
//   ASSERT_EQ(vec2.data(), ptr1);
// }

// TEST(VectorTest, swap_with_alloc) {
//   sp::pool_allocator<not_safe> al1(20);
//   sp::pool_allocator<not_safe> al2(66);

//   TargetVector<not_safe, sp::pool_allocator<not_safe>> vec1(10,
//   not_safe("one"),
//                                                             al1);
//   TargetVector<not_safe, sp::pool_allocator<not_safe>> vec2(33,
//   not_safe("two"),
//                                                             al2);

//   TargetVector<not_safe, sp::pool_allocator<not_safe>> exp1(vec2);
//   TargetVector<not_safe, sp::pool_allocator<not_safe>> exp2(vec1);

//   not_safe *ptr1 = vec1.data();
//   not_safe *ptr2 = vec2.data();

//   vec1.swap(vec2);

//   ASSERT_EQ(al1.leftover(), 0);
//   ASSERT_EQ(al2.leftover(), 0);
//   ASSERT_EQ(vec1, exp1);
//   ASSERT_EQ(vec2, exp2);
//   ASSERT_EQ(vec1.data(), ptr2);
//   ASSERT_EQ(vec2.data(), ptr1);
//   ASSERT_EQ(vec1.get_allocator(), al2);
//   ASSERT_EQ(vec2.get_allocator(), al1);
// }

// //==============================================================================
// // cmp and random access
// TEST(VectorTest, comparison_1) {
//   int size = uid(gen);
//   const TargetVector<not_safe> vec1(size, not_safe("equal"));
//   const TargetVector<not_safe> vec2(size, not_safe("equal"));

//   ASSERT_TRUE(vec1 == vec2);
//   ASSERT_FALSE(vec1 != vec2);
//   ASSERT_TRUE(vec2 == vec1);
//   ASSERT_FALSE(vec2 != vec1);
// }

// TEST(VectorTest, comparison_2) {
//   const TargetVector<not_safe> vec1(uid(gen), not_safe("not equal"));
//   const TargetVector<not_safe> vec2(uid(gen), not_safe("equal"));

//   ASSERT_FALSE(vec1 == vec2);
//   ASSERT_TRUE(vec1 != vec2);
//   ASSERT_FALSE(vec2 == vec1);
//   ASSERT_TRUE(vec2 != vec1);
// }

// TEST(VectorTest, comparison_3) {
//   const TargetVector<not_safe> vec1(uid(gen), not_safe("not equal"));
//   const TargetVector<not_safe> vec2;

//   ASSERT_FALSE(vec1 == vec2);
//   ASSERT_TRUE(vec1 != vec2);
//   ASSERT_FALSE(vec2 == vec1);
//   ASSERT_TRUE(vec2 != vec1);
// }

// TEST(VectorTest, comparison_self) {
//   const TargetVector<not_safe> vec(uid(gen), not_safe("not equal"));

//   ASSERT_TRUE(vec == vec);
//   ASSERT_FALSE(vec != vec);
// }

// TEST(VectorTest, comparison_empty) {
//   const TargetVector<not_safe> vec1;
//   const TargetVector<not_safe> vec2;

//   ASSERT_TRUE(vec1 == vec2);
//   ASSERT_FALSE(vec1 != vec2);
// }

// TEST(VectorTest, random_access) {
//   TargetVector<int> vec(uid(gen));
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec[i], 0);
//     vec[i] = i + 1;
//   }
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec[i], i + 1);
//   }
// }

// TEST(VectorTest, random_access_bounds) {
//   TargetVector<int> vec(uid(gen));
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec.at(i), 0);
//     vec.at(i) = i + 1;
//   }
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec.at(i), i + 1);
//   }
//   ASSERT_THROW(vec.at(-1), std::out_of_range);
//   ASSERT_THROW(vec.at(static_cast<int>(vec.size())), std::out_of_range);
// }

// TEST(VectorTest, front_back_access) {
//   TargetVector<int> vec(uid(gen));
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec[i], 0);
//     vec[i] = i + 1;
//   }
//   ASSERT_EQ(vec.front(), 1);
//   ASSERT_EQ(vec.back(), static_cast<int>(vec.size()));
// }

// TEST(VectorTest, data_access) {
//   const TargetVector<int> vec(uid(gen), 666);
//   const int *ptr = vec.data();
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) ASSERT_EQ(ptr[i],
//   666);
// }

// TEST(VectorTest, iterators) {
//   TargetVector<int> vec{0, 1, 2, 3, 4, 5};
//   TargetVector<int>::iterator it = vec.begin();
//   for (int i = 0; it != vec.end(); ++it, ++i) ASSERT_EQ(*it, vec[i]);
//   it = vec.begin() + 3;
//   *it = 10;
//   ASSERT_EQ(vec[3], 10);
// }

// TEST(VectorTest, reverse_iterators) {
//   TargetVector<int> vec{0, 1, 2, 3, 4, 5};
//   TargetVector<int>::reverse_iterator it = vec.rbegin();
//   for (int i = 5; it != vec.rend(); ++it, --i) ASSERT_EQ(*it, vec[i]);
//   it = vec.rbegin() + 3;
//   *it = 10;
//   ASSERT_EQ(vec[2], 10);
// }

// //==============================================================================
// // capacity
// TEST(VectorTest, assign_gt_not_safe_1) {
//   int size = 322;
//   TargetVector<not_safe> vec(10);

//   vec.assign(size, not_safe("tm"));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe("tm"));
//   }
// }

// TEST(VectorTest, assign_gt_not_safe_2) {
//   int size = 13;
//   TargetVector<not_safe> vec(10);

//   vec.assign(size, not_safe("nm"));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe("nm"));
//   }
// }

// TEST(VectorTest, assign_lt_not_safe_1) {
//   int size = 10;
//   TargetVector<not_safe> vec(322);
//   not_safe *ptr = vec.data();

//   vec.assign(size, not_safe("tm"));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   ASSERT_EQ(vec.data(), ptr);
//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe("tm"));
//     ASSERT_EQ(ob.birth, constructed::kDef);
//   }
// }

// TEST(VectorTest, assign_lt_not_safe_2) {
//   int size = 10;
//   TargetVector<not_safe> vec(13);
//   not_safe *ptr = vec.data();

//   vec.assign(size, not_safe("nm"));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   ASSERT_EQ(vec.data(), ptr);

//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe("nm"));
//   }
// }

// TEST(VectorTest, assign_gt_no_realloc) {
//   TargetVector<safe> vec(10);

//   vec.assign(322, safe("tm"));
//   safe *ptr = vec.data();
//   vec.assign(10, safe("tm"));
//   vec.assign(16, safe("tm"));

//   ASSERT_EQ(vec.size(), 16);
//   ASSERT_EQ(vec.capacity(), 322);
//   ASSERT_EQ(vec.data(), ptr);
//   for (const safe &ob : vec) {
//     ASSERT_EQ(ob, safe("tm"));
//   }
// }

// TEST(VectorTest, assign_throwing) {
//   throwing::count = 0;
//   int size = uid(gen);
//   TargetVector<throwing> vec(size);

//   ASSERT_ANY_THROW(vec.assign(7, throwing()));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (const throwing &ob : vec) {
//     ASSERT_EQ(ob, throwing());
//   }
// }

// TEST(VectorTest, assign_list_safe) {
//   TargetVector<safe> vec(uid(gen));

//   vec.assign({safe(), safe(), safe(), safe(), safe()});

//   for (const safe &ob : vec) {
//     ASSERT_EQ(ob, safe());
//   }
// }

// TEST(VectorTest, assign_random) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<safe> vec(uid(gen));

//     vec.assign(uid(gen), safe("ass"));

//     for (const safe &ob : vec) {
//       ASSERT_EQ(ob, safe("ass"));
//     }
//   }
// }

// TEST(VectorTest, assign_list_not_safe) {
//   TargetVector<not_safe> vec(uid(gen));

//   vec.assign({not_safe(), not_safe(), not_safe(), not_safe(), not_safe()});

//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe());
//   }
// }

// TEST(VectorTest, assign_list_throwing) {
//   throwing::count = 0;
//   int size = uid(gen);
//   TargetVector<throwing> vec(size);

//   ASSERT_ANY_THROW(vec.assign({throwing("not default"), throwing("not
//   default"),
//                                throwing("not default"), throwing("not
//                                default"), throwing("not default")}));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (int i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, assign_invalid_count) {
//   TargetVector<safe> vec(uid(gen));

//   ASSERT_THROW(vec.assign(-uid(gen), safe()), std::length_error);
// }

// TEST(VectorTest, assign_big_count) {
//   TargetVector<safe> vec(uid(gen));
//   std::allocator<safe> al;
//   std::size_t max =
//   std::allocator_traits<std::allocator<safe>>::max_size(al);
//   ASSERT_THROW(vec.assign(max + 1, safe()), std::length_error);
// }

// TEST(VectorTest, reserve_expand_safe) {
//   int size = 10;
//   int re_cap = 40;

//   TargetVector<safe> vec(size);

//   vec.reserve(re_cap);
//   ASSERT_EQ(size, static_cast<int>(vec.size()));
//   ASSERT_EQ(re_cap, vec.capacity());
//   for (const safe &ob : vec) {
//     ASSERT_EQ(ob, safe());
//     ASSERT_EQ(ob.birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, reserve_expand_not_safe) {
//   int size = 10;
//   int re_cap = 40;

//   TargetVector<not_safe> vec(size);

//   vec.reserve(re_cap);
//   ASSERT_EQ(size, static_cast<int>(vec.size()));
//   ASSERT_EQ(re_cap, vec.capacity());
//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe());
//     ASSERT_EQ(ob.birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, reserve_expand_throwing) {
//   throwing::count = 0;
//   int size = 5;
//   TargetVector<throwing> vec(size);

//   int old_cap = vec.capacity();

//   ASSERT_ANY_THROW(vec.reserve(17));

//   ASSERT_EQ(size, static_cast<int>(vec.size()));
//   ASSERT_EQ(old_cap, vec.capacity());
//   ASSERT_EQ(vec.back().birth, constructed::kDef);
//   ASSERT_EQ(vec.front().birth, constructed::kDef);
// }

// TEST(VectorTest, reserve_shrink) {
//   throwing::count = 0;

//   int size = 10;
//   TargetVector<throwing> vec(size);
//   throwing *ptr = vec.data();
//   vec.reserve(7);

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   ASSERT_EQ(vec.capacity(), size);
//   ASSERT_EQ(vec.data(), ptr);
//   ASSERT_EQ(vec.back().birth, constructed::kDef);
//   ASSERT_EQ(vec.front().birth, constructed::kDef);
// }

// TEST(VectorTest, reserve_invalid) {
//   TargetVector<safe> vec1(uid(gen));
//   ASSERT_THROW(vec1.reserve(-1), std::length_error);

//   TargetVector<not_safe> vec2(uid(gen));
//   ASSERT_THROW(vec2.reserve(-1), std::length_error);

//   TargetVector<throwing> vec3(uid(gen));
//   ASSERT_THROW(vec3.reserve(-1), std::length_error);
// }

// TEST(VectorTest, reserve_random) {
//   for (int i = 0; i < loop; ++i) {
//     int size = uid(gen);
//     int re_cap = uid(gen);

//     TargetVector<not_safe> vec(size, not_safe("rand"));

//     vec.reserve(re_cap);

//     ASSERT_EQ(size, static_cast<int>(vec.size()));
//     ASSERT_LE(re_cap, vec.capacity());
//     for (const not_safe &ob : vec) {
//       ASSERT_EQ(ob, not_safe("rand"));
//     }
//   }
// }

// TEST(VectorTest, stf_safe) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<safe> vec(uid(gen));

//     vec.reserve(vec.size() + uid(gen));
//     vec.shrink_to_fit();
//     ASSERT_EQ(vec.size(), vec.capacity());
//     ASSERT_EQ(vec.back().birth, constructed::kMove);
//     ASSERT_EQ(vec.front().birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, stf_not_safe) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<not_safe> vec(uid(gen));

//     vec.reserve(vec.size() + uid(gen));
//     vec.shrink_to_fit();
//     ASSERT_EQ(vec.size(), vec.capacity());
//     ASSERT_EQ(vec.back().birth, constructed::kCopy);
//     ASSERT_EQ(vec.front().birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, clear) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<not_safe> vec(uid(gen), not_safe("dirty"));

//     vec.clear();
//     ASSERT_EQ(vec.size(), 0);
//   }
// }

// TEST(VectorTest, resize_expand_not_safe) {
//   int size = 10;
//   int re_size = 15;
//   TargetVector<not_safe> vec(size, not_safe("default"));

//   vec.resize(re_size, not_safe("appended"));

//   ASSERT_EQ(re_size, static_cast<int>(vec.size()));
//   for (int j = 0; j < size; ++j) {
//     ASSERT_EQ(vec[j], not_safe("default"));
//     ASSERT_EQ(vec[j].birth, constructed::kCopy);
//   }
//   for (int j = size; j < re_size; ++j) {
//     ASSERT_EQ(vec[j], not_safe("appended"));
//     ASSERT_EQ(vec[j].birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, resize_expand_safe) {
//   int size = 10;
//   int re_size = 77;
//   TargetVector<safe> vec(size, safe("default"));

//   vec.resize(re_size, safe("appended"));

//   ASSERT_EQ(re_size, static_cast<int>(vec.size()));
//   for (int j = 0; j < size; ++j) {
//     ASSERT_EQ(vec[j], safe("default"));
//     ASSERT_EQ(vec[j].birth, constructed::kMove);
//   }
//   for (int j = size; j < re_size; ++j) {
//     ASSERT_EQ(vec[j], safe("appended"));
//     ASSERT_EQ(vec[j].birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, resize_expand_throwing) {
//   throwing::count = 0;
//   int size = 3;
//   TargetVector<throwing> vec(size);
//   ASSERT_ANY_THROW(vec.resize(10, throwing("appended")));
//   ASSERT_EQ(size, static_cast<int>(vec.size()));
//   for (const throwing &ob : vec) {
//     ASSERT_EQ(ob, throwing());
//     ASSERT_EQ(ob.birth, constructed::kDef);
//   }

//   throwing::count = 3;
//   ASSERT_ANY_THROW(vec.resize(10, throwing("appended")));
//   ASSERT_EQ(size, static_cast<int>(vec.size()));
//   for (const throwing &ob : vec) {
//     ASSERT_EQ(ob, throwing());
//     ASSERT_EQ(ob.birth, constructed::kDef);
//   }
// }

// TEST(VectorTest, resize_shrink) {
//   int size = 77;
//   int re_size = 23;
//   TargetVector<not_safe> vec(size);
//   int old_cap = vec.capacity();
//   not_safe *ptr = vec.data();

//   vec.resize(re_size);

//   ASSERT_EQ(re_size, static_cast<int>(vec.size()));
//   ASSERT_EQ(old_cap, vec.capacity());
//   ASSERT_EQ(ptr, vec.data());

//   for (const not_safe &ob : vec) {
//     ASSERT_EQ(ob, not_safe());
//     ASSERT_EQ(ob.birth, constructed::kDef);
//   }
// }

// TEST(VectorTest, resize_shrink_throwing) {
//   throwing::count = 0;
//   int re_size = 5;
//   TargetVector<throwing> vec(17);
//   int old_cap = vec.capacity();

//   ASSERT_NO_THROW(vec.resize(re_size));
//   ASSERT_EQ(re_size, static_cast<int>(vec.size()));
//   ASSERT_EQ(old_cap, vec.capacity());

//   for (const throwing &ob : vec) {
//     ASSERT_EQ(ob, throwing());
//     ASSERT_EQ(ob.birth, constructed::kDef);
//   }
// }

// TEST(VectorTest, resize_expand_no_realloc) {
//   TargetVector<safe> vec(40);
//   safe *ptr = vec.data();

//   vec.resize(10, safe("appended"));
//   vec.resize(30, safe("appended"));

//   ASSERT_EQ(vec.size(), 30);
//   ASSERT_EQ(vec.capacity(), 40);
//   ASSERT_EQ(vec.data(), ptr);

//   for (int j = 0; j < 10; ++j) {
//     ASSERT_EQ(vec[j], safe("default"));
//     ASSERT_EQ(vec[j].birth, constructed::kDef);
//   }
//   for (int j = 10; j < 30; ++j) {
//     ASSERT_EQ(vec[j], safe("appended"));
//   }
// }

// TEST(VectorTest, resize_zero) {
//   TargetVector<not_safe> vec(uid(gen));
//   int old_cap = vec.capacity();
//   not_safe *ptr = vec.data();
//   vec.resize(0);
//   ASSERT_EQ(0, vec.size());
//   ASSERT_EQ(old_cap, vec.capacity());
//   ASSERT_EQ(ptr, vec.data());
// }

// TEST(VectorTest, resize_invalid) {
//   TargetVector<not_safe> vec(uid(gen));
//   ASSERT_THROW(vec.resize(-1, not_safe("dolbaeb")), std::length_error);
// }

// TEST(VectorTest, resize_random) {
//   for (int i = 0; i < loop; ++i) {
//     int size = uid(gen);
//     int re_size = uid(gen);
//     TargetVector<not_safe> vec(size, not_safe("default"));

//     vec.resize(re_size, not_safe("appended"));

//     ASSERT_EQ(re_size, static_cast<int>(vec.size()));
//     for (int j = 0; j < size && j < re_size; ++j) {
//       ASSERT_EQ(vec[j], not_safe("default"));
//     }
//     for (int j = std::min(size, re_size); j < re_size; ++j) {
//       ASSERT_EQ(vec[j], not_safe("appended"));
//     }
//   }
// }
// //==============================================================================
// // modifiers

// TEST(VectorTest, insert_continious) {
//   not_safe insert_val("inserted");
//   TargetVector<not_safe> vec(60);
//   int insert = 0;
//   for (int i = 0; i < loop; ++i, insert += 4) {
//     auto pos = vec.insert(vec.begin() + insert, insert_val);
//     ASSERT_EQ(pos, vec.begin() + insert);
//     ASSERT_EQ(*pos, insert_val);
//   }
// }

// TEST(VectorTest, insert_not_safe) {
//   TargetVector<not_safe> vec(60);
//   int insert = 8;
//   auto pos = vec.insert(vec.begin() + insert, not_safe("inserted"));
//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, not_safe("inserted"));
//   for (auto i = vec.begin(); i != pos; ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
//   for (auto i = pos + 1; i != vec.end(); ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, insert_safe) {
//   TargetVector<safe> vec(60);
//   int insert = 47;
//   auto pos = vec.insert(vec.begin() + insert, safe("inserted"));
//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, safe("inserted"));
//   for (auto i = vec.begin(); i != pos; ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
//   for (auto i = pos + 1; i != vec.end(); ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, insert_empty) {
//   safe insert_val("inserted");
//   TargetVector<safe> vec;
//   auto insert_pos = vec.insert(vec.begin(), insert_val);
//   ASSERT_EQ(vec.size(), 1);
//   ASSERT_EQ(vec.capacity(), 1);
//   ASSERT_NE(vec.data(), nullptr);
//   ASSERT_EQ(insert_pos, vec.begin());
//   ASSERT_EQ(*insert_pos, insert_val);
// }

// TEST(VectorTest, insert_throwing) {
//   throwing::count = 0;
//   int size = 4;
//   throwing insert_val("inserted");
//   TargetVector<throwing> vec(size);
//   ASSERT_ANY_THROW(vec.insert(vec.begin(), throwing("face")));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   throwing::count = 4;
//   ASSERT_ANY_THROW(vec.insert(vec.begin(), throwing("in your")));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, insert_safe_counted) {
//   int insert = 47;
//   int size = 60;
//   int count = 8;
//   TargetVector<safe> vec(size);

//   auto pos = vec.insert(vec.begin() + insert, count, safe("inserted"));

//   ASSERT_EQ(static_cast<int>(vec.size()), size + count);
//   ASSERT_EQ(vec.capacity(), size * 2);
//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, safe("inserted"));
//   auto i = vec.begin();

//   for (; i != pos; ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
//   for (; i != pos + count; ++i) {
//     ASSERT_EQ(*i, safe("inserted"));
//   }
//   for (; i != vec.end(); ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, insert_not_safe_counted) {
//   int insert = 8;
//   int size = 60;
//   int count = 67;
//   TargetVector<not_safe> vec(size);

//   auto pos = vec.insert(vec.begin() + insert, count, not_safe("inserted"));

//   ASSERT_EQ(static_cast<int>(vec.size()), size + count);
//   ASSERT_EQ(vec.capacity(), size + count);
//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, not_safe("inserted"));
//   auto i = vec.begin();

//   for (; i != pos; ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
//   for (; i != pos + count; ++i) {
//     ASSERT_EQ(*i, not_safe("inserted"));
//   }
//   for (; i != vec.end(); ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, insert_counted_no_realloc) {
//   TargetVector<safe> vec(77);
//   safe *ptr = vec.data();
//   vec.resize(25);

//   int insert = 8;
//   int count = 9;

//   auto pos = vec.insert(vec.begin() + insert, count, safe("inserted"));

//   ASSERT_EQ(static_cast<int>(vec.size()), 25 + count);
//   ASSERT_EQ(vec.capacity(), 77);
//   ASSERT_EQ(vec.data(), ptr);
//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, safe("inserted"));
//   auto i = vec.begin();

//   for (; i != pos; ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kDef);
//   }
//   for (; i != pos + count; ++i) {
//     ASSERT_EQ(*i, safe("inserted"));
//   }
// }

// TEST(VectorTest, insert_counted_throwing) {
//   throwing::count = 0;
//   int size = 4;
//   throwing insert_val("inserted");
//   TargetVector<throwing> vec(size);
//   throwing *ptr = vec.data();

//   ASSERT_ANY_THROW(vec.insert(vec.begin(), 4, throwing("face")));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   ASSERT_EQ(vec.capacity(), size);
//   ASSERT_EQ(vec.data(), ptr);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   throwing::count = 4;

//   ASSERT_ANY_THROW(vec.insert(vec.begin(), 4, throwing("in your")));

//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   ASSERT_EQ(vec.capacity(), size);
//   ASSERT_EQ(vec.data(), ptr);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, insert_range_safe) {
//   int size = 60;
//   TargetVector<safe> vec(size);
//   std::vector<safe> range(size, safe("inserted"));

//   int start = 8;
//   int finish = 24;
//   int count = finish - start;

//   int insert = 16;

//   auto pos = vec.insert(vec.begin() + insert, range.begin() + start,
//                         range.begin() + finish);

//   ASSERT_EQ(static_cast<int>(vec.size()), size + count);
//   ASSERT_EQ(pos, vec.begin() + insert);

//   auto i = vec.begin();

//   for (; i != pos; ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
//   for (; i != pos + count; ++i) {
//     ASSERT_EQ(*i, safe("inserted"));
//   }
//   for (; i != vec.end(); ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, insert_range_not_safe) {
//   int size = 60;
//   TargetVector<not_safe> vec(size);
//   std::vector<not_safe> range(size, not_safe("inserted"));

//   int start = 8;
//   int finish = 24;
//   int count = finish - start;

//   int insert = 16;

//   auto pos = vec.insert(vec.begin() + insert, range.begin() + start,
//                         range.begin() + finish);

//   ASSERT_EQ(static_cast<int>(vec.size()), size + count);
//   ASSERT_EQ(pos, vec.begin() + insert);

//   auto i = vec.begin();

//   for (; i != pos; ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
//   for (; i != pos + count; ++i) {
//     ASSERT_EQ(*i, not_safe("inserted"));
//   }
//   for (; i != vec.end(); ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, insert_range_no_realloc) {
//   int size = 60;
//   TargetVector<safe> vec(size);
//   std::vector<safe> range(size, safe("inserted"));

//   vec.resize(10);

//   int start = 8;
//   int finish = 24;
//   int count = finish - start;

//   int insert = 6;

//   auto pos = vec.insert(vec.begin() + insert, range.begin() + start,
//                         range.begin() + finish);

//   ASSERT_EQ(static_cast<int>(vec.size()), 10 + count);
//   ASSERT_EQ(pos, vec.begin() + insert);

//   auto i = vec.begin();

//   for (; i != pos; ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kDef);
//   }
//   for (; i != pos + count; ++i) {
//     ASSERT_EQ(*i, not_safe("inserted"));
//   }
// }

// TEST(VectorTest, insert_range_throwing) {
//   throwing::count = 0;
//   int size = 4;
//   TargetVector<throwing> vec(size);
//   std::vector<throwing> range(10);
//   int start = 2;
//   int finish = start + 5;
//   ASSERT_ANY_THROW(
//       vec.insert(vec.begin(), range.begin() + start, range.begin() +
//       finish));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   throwing::count = 0;
//   ASSERT_ANY_THROW(
//       vec.insert(vec.begin(), range.begin() + start, range.begin() +
//       finish));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, insert_list_safe) {
//   for (int i = 0; i < loop; ++i) {
//     int size = uid(gen) + 3;
//     TargetVector<safe> vec(size);

//     int insert = 2;

//     auto pos =
//         vec.insert(vec.begin() + insert,
//                    {safe("inserted"), safe("inserted"), safe("inserted")});
//     ASSERT_EQ(static_cast<int>(vec.size()), size + 3);
//     ASSERT_EQ(pos, vec.begin() + insert);
//     auto it = vec.begin();

//     for (; it != pos; ++it) {
//       ASSERT_NE(*it, safe("inserted"));
//       ASSERT_EQ(it->birth, constructed::kMove);
//     }
//     for (; it != pos + 3; ++it) {
//       ASSERT_EQ(*it, safe("inserted"));
//     }
//     for (; it != vec.end(); ++it) {
//       ASSERT_NE(*it, safe("inserted"));
//       ASSERT_EQ(it->birth, constructed::kMove);
//     }
//   }
// }

// TEST(VectorTest, insert_list_not_safe) {
//   for (int i = 0; i < loop; ++i) {
//     int size = uid(gen);
//     TargetVector<not_safe> vec(size);
//     std::uniform_int_distribution<int> uid_vec(1, size);

//     int insert = uid_vec(gen);

//     auto pos = vec.insert(
//         vec.begin() + insert,
//         {not_safe("inserted"), not_safe("inserted"), not_safe("inserted")});
//     ASSERT_EQ(static_cast<int>(vec.size()), size + 3);
//     ASSERT_EQ(pos, vec.begin() + insert);
//     auto it = vec.begin();

//     for (; it != pos; ++it) {
//       ASSERT_NE(*it, not_safe("inserted"));
//       ASSERT_EQ(it->birth, constructed::kCopy);
//     }
//     for (; it != pos + 3; ++it) {
//       ASSERT_EQ(*it, not_safe("inserted"));
//     }
//     for (; it != vec.end(); ++it) {
//       ASSERT_NE(*it, not_safe("inserted"));
//       ASSERT_EQ(it->birth, constructed::kCopy);
//     }
//   }
// }

// TEST(VectorTest, erase_not_safe) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<not_safe> vec(uid(gen));
//     std::uniform_int_distribution<int> uid_vec(0, vec.size() - 1);
//     auto pos = vec.begin() + uid_vec(gen);
//     pos = vec.insert(pos, not_safe("inserted"));
//     vec.erase(pos);
//     for (const safe &ob : vec) {
//       ASSERT_NE(ob, not_safe("inserted"));
//       ASSERT_EQ(ob.birth, constructed::kCopy);
//     }
//   }
// }

// TEST(VectorTest, erase_safe) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<safe> vec(uid(gen));
//     std::uniform_int_distribution<int> uid_vec(0, vec.size() - 1);
//     auto pos = vec.begin() + uid_vec(gen);
//     pos = vec.insert(pos, safe("inserted"));
//     vec.erase(pos);
//     for (const safe &ob : vec) {
//       ASSERT_NE(ob, safe("inserted"));
//       ASSERT_NE(ob.birth, constructed::kCopy);
//     }
//   }
// }

// TEST(VectorTest, erase_throwing) {
//   throwing::count = 0;
//   int size = uid(gen) + 10;
//   TargetVector<throwing> vec(size);
//   ASSERT_ANY_THROW(vec.erase(vec.begin() + 7));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   ASSERT_ANY_THROW(vec.erase(vec.begin() + 2));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, erase_to_empty) {
//   int size = uid(gen);
//   TargetVector<safe> vec(size);
//   for (int i = 0; i < size; ++i) {
//     vec.erase(vec.begin());
//   }
//   ASSERT_EQ(vec.size(), 0);
// }

// TEST(VectorTest, erase_last) {
//   int size = uid(gen);
//   TargetVector<safe> vec(size);
//   auto pos = vec.end() - 1;
//   pos = vec.erase(pos);
//   ASSERT_EQ(pos, vec.end());
//   ASSERT_EQ(static_cast<int>(vec.size()), size - 1);
// }

// TEST(VectorTest, erase_range_safe) {
//   TargetVector<safe> vec(10);
//   int start = 2;
//   int finish = 6;
//   int n_size = vec.size() - (finish - start);
//   int cap = vec.capacity();

//   auto it = vec.erase(vec.begin() + start, vec.begin() + finish);

//   ASSERT_EQ(n_size, static_cast<int>(vec.size()));
//   ASSERT_EQ(vec.capacity(), cap);
//   ASSERT_EQ(it, vec.begin() + start);
//   for (const safe &ob : vec) {
//     ASSERT_NE(ob.birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, erase_range_not_safe) {
//   TargetVector<not_safe> vec{not_safe("no erase"), not_safe("no erase"),
//                              not_safe("erase"),    not_safe("erase"),
//                              not_safe("erase"),    not_safe("erase"),
//                              not_safe("no erase"), not_safe("no erase")};
//   int start = 2;
//   int finish = 6;
//   int n_size = vec.size() - (finish - start);
//   int cap = vec.capacity();

//   auto it = vec.erase(vec.begin() + start, vec.begin() + finish);

//   ASSERT_EQ(n_size, static_cast<int>(vec.size()));
//   ASSERT_EQ(vec.capacity(), cap);
//   ASSERT_EQ(it, vec.begin() + start);
//   for (const not_safe &ob : vec) {
//     ASSERT_NE(ob, not_safe("erase"));
//   }
// }

// TEST(VectorTest, erase_range_throwing) {
//   TargetVector<throwing> vec(10);
//   int start = 2;
//   int finish = 4;
//   int cap = vec.capacity();

//   ASSERT_ANY_THROW(vec.erase(vec.begin() + start, vec.begin() + finish));

//   ASSERT_EQ(vec.size(), 10);
//   ASSERT_EQ(vec.capacity(), cap);
// }

// TEST(VectorTest, erase_range_end) {
//   for (int i = 0; i < loop; ++i) {
//     TargetVector<safe> vec(uid(gen), safe("default"));
//     std::uniform_int_distribution<int> uid_vec(0, vec.size());
//     auto pos = vec.begin() + uid_vec(gen);
//     int n_size = vec.size() - (vec.end() - pos);

//     pos = vec.erase(pos, vec.end());

//     ASSERT_EQ(pos, vec.end());
//     ASSERT_EQ(n_size, static_cast<int>(vec.size()));
//   }
// }

// TEST(VectorTest, erase_range_entire) {
//   TargetVector<safe> vec(uid(gen));
//   vec.erase(vec.begin(), vec.end());
//   ASSERT_EQ(vec.size(), 0);
// }

// TEST(VectorTest, erase_range_empty) {
//   for (int i = 0; i < loop; ++i) {
//     int size = uid(gen) + 1;
//     TargetVector<safe> vec(size);
//     auto pos = vec.erase(vec.begin() + 1, vec.begin() + 1);
//     ASSERT_EQ(pos, vec.begin() + 1);
//     ASSERT_EQ(static_cast<int>(vec.size()), size);
//   }
// }

// TEST(VectorTest, push_back_not_safe) {
//   not_safe push_value("pushed");
//   TargetVector<not_safe> vec(uid(gen));

//   vec.push_back(push_value);

//   ASSERT_EQ(vec.back(), push_value);
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec[i].birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, push_back_safe) {
//   TargetVector<safe> vec(uid(gen));

//   vec.push_back(safe("pushed"));

//   ASSERT_EQ(vec.back(), safe("pushed"));
//   for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
//     ASSERT_EQ(vec[i].birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, push_back_continious) {
//   TargetVector<safe> vec(uid(gen));

//   for (int i = 0; i < loop; ++i) {
//     vec.push_back(safe("pushed"));
//     ASSERT_EQ(vec.back(), safe("pushed"));
//     ASSERT_EQ(vec.back().birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, push_back_throwing) {
//   throwing::count = 0;
//   int size = 4;
//   throwing push_value("pushed");
//   TargetVector<throwing> vec(size);
//   ASSERT_ANY_THROW(vec.push_back(push_value));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   throwing::count = 4;
//   ASSERT_ANY_THROW(vec.push_back(push_value));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, pop_back_same_vector) {
//   TargetVector<safe> vec(77, safe("default"));
//   for (int i = 0; i < 45; ++i) {
//     vec.pop_back();
//   }
//   ASSERT_EQ(vec.size(), 77 - 45);
// }

// TEST(VectorTest, pop_back_to_empty) {
//   int size = uid(gen);
//   TargetVector<safe> vec(size);
//   for (int i = 0; i < size; ++i) {
//     vec.pop_back();
//   }
//   ASSERT_EQ(vec.size(), 0);
// }

// TEST(VectorTest, emplace_not_safe) {
//   TargetVector<not_safe> vec(54);
//   not_safe value("inserted");
//   int insert = 31;

//   auto pos = vec.emplace(vec.begin() + insert, value);

//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, not_safe("inserted"));
//   ASSERT_EQ(pos->birth, constructed::kCopy);

//   for (auto i = vec.begin(); i != pos; ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
//   for (auto i = pos + 1; i != vec.end(); ++i) {
//     ASSERT_NE(*i, not_safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, emplace_safe) {
//   TargetVector<safe> vec(54);
//   int insert = 31;

//   auto pos = vec.emplace(vec.begin() + insert, safe("inserted"));

//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, safe("inserted"));
//   ASSERT_EQ(pos->birth, constructed::kMove);

//   for (auto i = vec.begin(); i != pos; ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
//   for (auto i = pos + 1; i != vec.end(); ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, emplace_no_realloc) {
//   TargetVector<safe> vec(54);
//   safe *ptr = vec.data();
//   vec.resize(50);

//   int insert = 31;

//   auto pos = vec.emplace(vec.begin() + insert, "inserted");

//   ASSERT_EQ(vec.data(), ptr);
//   ASSERT_EQ(pos, vec.begin() + insert);
//   ASSERT_EQ(*pos, safe("inserted"));
//   ASSERT_EQ(pos->birth, constructed::kDef);

//   for (auto i = vec.begin(); i != pos; ++i) {
//     ASSERT_NE(*i, safe("inserted"));
//     ASSERT_EQ(i->birth, constructed::kDef);
//   }
//   ASSERT_EQ(vec.back().birth, constructed::kMove);
// }

// TEST(VectorTest, emplace_no_def) {
//   TargetVector<no_def> vec;
//   for (int i = 0; i < loop; ++i) {
//     auto insert_pos = vec.emplace(vec.begin(), "inserted");
//     ASSERT_EQ(*insert_pos, no_def("inserted"));
//     ASSERT_EQ(insert_pos->birth, constructed::kParam);
//   }
// }

// TEST(VectorTest, emplace_throwing) {
//   throwing::count = 0;
//   int size = 4;
//   TargetVector<throwing> vec(size);
//   throwing value("pushed");
//   ASSERT_ANY_THROW(vec.emplace(vec.begin(), value));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   throwing::count = 4;
//   ASSERT_ANY_THROW(vec.emplace(vec.begin(), value));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }

// TEST(VectorTest, emplace_back_from_begin_safe) {
//   TargetVector<safe> vec(uid(gen), safe("default"));
//   vec.emplace_back(std::move(vec.front()));
//   ASSERT_EQ(vec.back(), safe("default"));
//   ASSERT_EQ(vec.back().birth, constructed::kMove);
//   ASSERT_EQ(vec.front().birth, constructed::kMove);
//   ASSERT_NE(vec.front(), safe());
// }

// TEST(VectorTest, emplace_back_continious_safe) {
//   int size = uid(gen);
//   TargetVector<safe> vec(size, safe("default"));
//   for (int i = 0; i < size; ++i) {
//     vec.emplace_back(std::move(vec[i]));
//   }
//   ASSERT_EQ(static_cast<int>(vec.size()), size * 2);
//   for (int i = 0; i < size; ++i) {
//     ASSERT_NE(vec[i], safe("default"));
//   }
//   for (int i = size; i < 2 * size; ++i) {
//     ASSERT_EQ(vec[i], safe("default"));
//   }
// }

// TEST(VectorTest, emplace_back_safe) {
//   TargetVector<safe> vec(uid(gen));

//   safe &val = vec.emplace_back("default");
//   ASSERT_EQ(val, safe("default"));
//   ASSERT_EQ(val.birth, constructed::kParam);
//   for (int i = 0; i < static_cast<int>(vec.size()) - 1; ++i) {
//     ASSERT_EQ(vec[i].birth, constructed::kMove);
//   }
// }

// TEST(VectorTest, emplace_back_not_safe) {
//   TargetVector<not_safe> vec(uid(gen));

//   not_safe &val = vec.emplace_back("default");
//   ASSERT_EQ(val, not_safe("default"));
//   ASSERT_EQ(val.birth, constructed::kParam);
//   for (int i = 0; i < static_cast<int>(vec.size()) - 1; ++i) {
//     ASSERT_EQ(vec[i].birth, constructed::kCopy);
//   }
// }

// TEST(VectorTest, emplace_back_no_realloc) {
//   TargetVector<safe> vec(56);
//   vec.resize(55);

//   safe &val = vec.emplace_back("default");
//   ASSERT_EQ(val, safe("default"));
//   ASSERT_EQ(val.birth, constructed::kParam);
//   for (int i = 0; i < static_cast<int>(vec.size()) - 1; ++i) {
//     ASSERT_EQ(vec[i].birth, constructed::kDef);
//   }
// }

// TEST(VectorTest, emplace_back_throwing) {
//   throwing::count = 0;
//   int size = 7;
//   TargetVector<throwing> vec(size);
//   ASSERT_ANY_THROW(vec.emplace(vec.begin(), "pushed"));
//   ASSERT_EQ(static_cast<int>(vec.size()), size);
//   for (auto i = 0; i < size; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }

//   throwing::count = 4;
//   vec.resize(6);

//   ASSERT_ANY_THROW(vec.emplace(vec.begin(), throwing("pushed")));
//   ASSERT_EQ(static_cast<int>(vec.size()), size - 1);
//   for (auto i = 0; i < size - 1; ++i) {
//     ASSERT_NO_THROW(vec.at(i));
//   }
// }
