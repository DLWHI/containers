#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <list>
#include <random>

#include <dlwhi/vector.h>
#include "test_helpers.h"

std::random_device ran_dev;
std::uniform_int_distribution<size_t> uid(1, 15);
std::mt19937 gen(ran_dev());


TEST(VectorTest, ctor_default) {
  dlwhi::vector<dummy> vec;

  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(vec.capacity(), 0);
  EXPECT_EQ(vec.data(), nullptr);
  EXPECT_THROW(vec.at(uid(gen)), std::out_of_range);
}

TEST(VectorTest, ctor_size) {
  size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec(random_size);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummy());
  EXPECT_EQ(vec.back(), dummy());
}

TEST(VectorTest, ctor_size_alloc_not_default) {
  size_t random_size = uid(gen);
  std::allocator<dummy> not_default;
  dlwhi::vector<dummy> vec(random_size, not_default);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummy());
  EXPECT_EQ(vec.back(), dummy());
}

TEST(VectorTest, ctor_size_value) {
  size_t random_size = uid(gen);
  std::allocator<dummy> not_default;
  dlwhi::vector<dummy> vec(random_size, dummy("not default"), not_default);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummy("not default"));
  EXPECT_EQ(vec.back(), dummy("not default"));
}

TEST(VectorTest, ctor_from_stl_vector) {
  size_t random_size = uid(gen);
  std::allocator<dummy> not_default;
  std::vector<dummy> from(random_size, dummy("not default"));

  dlwhi::vector<dummy> vec(from.begin(), from.end(), not_default);
  EXPECT_EQ(vec.size(), from.size());
  EXPECT_EQ(vec.capacity(), from.capacity());
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), from.front());
  EXPECT_EQ(vec.back(), from.back());
}

TEST(VectorTest, ctor_from_stl_list) {
  size_t random_size = uid(gen);
  std::allocator<dummy> not_default;
  std::list<dummy> from(random_size, dummy("not default"));

  dlwhi::vector<dummy> vec(from.begin(), from.end(), not_default);
  
  EXPECT_EQ(vec.size(), from.size());
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), from.front());
  EXPECT_EQ(vec.back(), from.back());
}

TEST(VectorTest, ctor_init_list) {
  std::initializer_list<std::string> list{
    "Karen",
    "Anastasia",
    "Alice",
    "Natalie",
    "Leyla",
    "Victoria"
  };

  std::allocator<std::string> not_default;
  dlwhi::vector<std::string> vec(list, not_default);
  EXPECT_EQ(vec.size(), list.size());
  EXPECT_EQ(vec.capacity(), list.size());
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), "Karen");
}

TEST(VectorTest, ctor_init_list_implicit) {
  dlwhi::vector<std::string> vec{
    "Karen",
    "Anastasia",
    "Alice",
    "Natalie",
    "Leyla",
    "Victoria"
  };

  EXPECT_EQ(vec.size(), 6);
  EXPECT_EQ(vec.capacity(), 6);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), "Karen");
}

TEST(VectorTest, ctor_copy) {
  size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2(vec1);

  EXPECT_EQ(vec1.size(), vec2.size());
  EXPECT_EQ(vec1.capacity(), vec2.capacity());
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec1.front(), vec2.front());
  EXPECT_EQ(vec1.back(), vec2.front());
}

TEST(VectorTest, ctor_move) {
  size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2(std::move(vec1));

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummy("not default"));
  EXPECT_EQ(vec2.back(), dummy("not default"));
}

TEST(VectorTest, ctor_move_alloc_not_default) {
  size_t random_size = uid(gen);
  state_allocator<dummy> not_default("not default");
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"), state_allocator<dummy>("default"));
  dlwhi::vector<dummy> vec2(std::move(vec1), not_default);

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummy("not default"));
  EXPECT_EQ(vec2.back(), dummy("not default"));
  EXPECT_EQ(vec2.get_allocator(), not_default);
}

TEST(VectorTest, ctor_ass_copy) {
  size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2;

  vec2 = vec1;

  EXPECT_EQ(vec1.size(), vec2.size());
  EXPECT_EQ(vec1.capacity(), vec2.capacity());
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec1.front(), vec2.front());
  EXPECT_EQ(vec1.back(), vec2.front());
}

TEST(VectorTest, ctor_ass_move) {
  size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2;

  vec2 = std::move(vec1);

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummy("not default"));
  EXPECT_EQ(vec2.back(), dummy("not default"));
}

// TEST(VectorTest, scenario_2) {
//   dlwhi::vector<dummy> dlwhivec(12);
//   std::vector<dummy> vec(12);
//   EXPECT_EQ(dlwhivec, vec);

//   dummy one("one");
//   dlwhivec.push_back(one);
//   vec.push_back(one);
//   EXPECT_TRUE(dlwhivec == vec);

//   dummy two("two");
//   dlwhivec.push_back(two);
//   vec.push_back(two);
//   EXPECT_TRUE(dlwhivec == vec);

//   dummy three("three");
//   dummy four("four");
//   dummy five("five");
//   dlwhivec.push_back(three);
//   dlwhivec.push_back(four);
//   dlwhivec.push_back(five);
//   vec.push_back(three);
//   vec.push_back(four);
//   vec.push_back(five);
//   EXPECT_TRUE(dlwhivec == vec);

//   dlwhivec.pop_back();
//   vec.pop_back();
//   EXPECT_TRUE(dlwhivec == vec);

//   dummy six("six");
//   dummy seven("seven");
//   dlwhivec.push_back(six);
//   vec.push_back(six);
//   dlwhivec.push_back(seven);
//   vec.push_back(seven);
//   EXPECT_TRUE(dlwhivec == vec);

//   dlwhivec.shrink_to_fit();
//   vec.shrink_to_fit();
//   EXPECT_TRUE(dlwhivec == vec);
// }

// TEST(VectorTest, scenario_3) {
//   dlwhi::vector<dummyMovable> dlwhivec(12, dummyMovable("was there"));
//   std::vector<dummyMovable> vec(12, dummyMovable("was there"));
//   EXPECT_EQ(dlwhivec, vec);

//   dlwhivec.push_back(dummyMovable("pushed1"));
//   vec.push_back(dummyMovable("pushed1"));
//   EXPECT_TRUE(dlwhivec == vec);

//   dlwhivec.push_back(dummyMovable("pushed2"));
//   vec.push_back(dummyMovable("pushed2"));
//   EXPECT_TRUE(dlwhivec == vec);

//   dlwhivec.push_back(dummyMovable("pushed3"));
//   vec.push_back(dummyMovable("pushed3"));
//   dlwhivec.push_back(dummyMovable("pushed4"));
//   vec.push_back(dummyMovable("pushed4"));
//   dlwhivec.push_back(dummyMovable("pushed5"));
//   vec.push_back(dummyMovable("pushed5"));
//   EXPECT_TRUE(dlwhivec == vec);

//   dlwhivec.pop_back();
//   vec.pop_back();
//   EXPECT_TRUE(dlwhivec == vec);

//   dummy six("six");
//   dummy seven("seven");
//   dlwhivec.push_back(dummyMovable("pushed6"));
//   vec.push_back(dummyMovable("pushed6"));
//   dlwhivec.push_back(dummyMovable("pushed7"));
//   vec.push_back(dummyMovable("pushed7"));
//   EXPECT_TRUE(dlwhivec == vec);

//   dlwhivec.reserve(14);
//   dlwhivec.reserve(14);
//   EXPECT_TRUE(dlwhivec == vec);
// }

// TEST(VectorTest, scenario_assign_1) {
//   dlwhi::vector<dummyMovable> dlwhivec(3);
//   std::vector<dummyMovable> vec(3);
//   EXPECT_EQ(vec, dlwhivec);

//   vec.assign({dummyMovable(), dummyMovable(), dummyMovable(), dummyMovable(),
//               dummyMovable()});
//   dlwhivec.assign({dummyMovable(), dummyMovable(), dummyMovable(), dummyMovable(),
//                  dummyMovable()});
//   EXPECT_EQ(vec, dlwhivec);

//   dummyMovable target("what");
//   dlwhivec.assign(17, target);
//   vec.assign(17, target);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.clear();
//   EXPECT_FALSE(vec == dlwhivec);
//   dlwhivec.assign(vec.begin(), vec.end());
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.clear();
//   EXPECT_ANY_THROW(dlwhivec.assign(vec.end(), vec.begin()));
// }

// TEST(VectorTest, scenario_assign_2) {
//   dlwhi::vector<dummy> dlwhivec(12);
//   std::vector<dummy> vec(12);
//   EXPECT_EQ(vec, dlwhivec);
//   dummy target("what");
//   dlwhivec.assign(5, target);
//   vec.assign(5, target);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.clear();
//   vec.clear();
//   EXPECT_EQ(vec, dlwhivec);
//   dlwhivec.assign(2, target);
//   vec.assign(2, target);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.resize(20);
//   vec.resize(20);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.assign(12, target);
//   vec.assign(12, target);
//   EXPECT_EQ(vec, dlwhivec);

//   EXPECT_ANY_THROW(dlwhivec.assign(-1, target));
// }

// TEST(VectorTest, scenario_insert) {
//   dlwhi::vector<dummyMovable> dlwhivec(12);
//   std::vector<dummyMovable> vec(12);

//   dummyMovable target("what");
//   auto dlwhiit = dlwhivec.begin() + 4;
//   auto it = vec.begin() + 4;

//   EXPECT_EQ(*dlwhiit, *it);
//   it++;
//   dlwhiit++;
//   EXPECT_EQ(*dlwhiit, *it);

//   it = vec.insert(it, target);
//   dlwhiit = dlwhivec.insert(dlwhiit, target);
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(dlwhivec, vec);

//   it = vec.insert(it, dummyMovable("inserted"));
//   dlwhiit = dlwhivec.insert(dlwhiit, dummyMovable("inserted"));
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(dlwhivec, vec);

//   it = vec.insert(vec.begin() + 7, dummyMovable("second inserted"));
//   dlwhiit = dlwhivec.insert(dlwhivec.begin() + 7, dummyMovable("second inserted"));
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(dlwhivec, vec);

//   it = vec.erase(it);
//   dlwhiit = dlwhivec.erase(dlwhiit);
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(dlwhivec, vec);
// }

// TEST(VectorTest, scenario_resize) {
//   dlwhi::vector<dummyMovable> dlwhivec{
//       dummyMovable("not default"), dummyMovable("not default"),
//       dummyMovable("not default"), dummyMovable("not default"),
//       dummyMovable("not default")};
//   std::vector<dummyMovable> vec{
//       dummyMovable("not default"), dummyMovable("not default"),
//       dummyMovable("not default"), dummyMovable("not default"),
//       dummyMovable("not default")};

//   dlwhivec.resize(33);
//   vec.resize(33);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.resize(17);
//   vec.resize(17);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.resize(3);
//   vec.resize(3);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.resize(7, dummyMovable("nd"));
//   vec.resize(7, dummyMovable("nd"));
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.resize(7);
//   vec.resize(7);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.resize(8, dummyMovable("that one is mine"));
//   vec.resize(8, dummyMovable("that one is mine"));
//   EXPECT_EQ(vec, dlwhivec);

//   EXPECT_ANY_THROW(dlwhivec.resize(-1));
// }

// TEST(VectorTest, scenario_reserve) {
//   dummyMovable target("this");
//   dlwhi::vector<dummyMovable> dlwhivec(12, target);
//   std::vector<dummyMovable> vec(12, target);

//   dlwhivec.reserve(6);
//   vec.reserve(6);
//   EXPECT_EQ(dlwhivec, vec);

//   dlwhivec.reserve(15);
//   vec.reserve(15);
//   EXPECT_EQ(dlwhivec, vec);

//   dlwhivec.reserve(0);
//   vec.reserve(0);
//   EXPECT_EQ(dlwhivec, vec);

//   EXPECT_ANY_THROW(dlwhivec.reserve(-1));
// }

// TEST(VectorTest, scenario_emplace) {
//   dlwhi::vector<dummyMovable> dlwhivec;
//   std::vector<dummyMovable> vec;
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.emplace_back(dummyMovable());
//   vec.emplace_back(dummyMovable());
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.emplace_back(dummyMovable(), dummyMovable(), dummyMovable());
//   vec.emplace_back(dummyMovable());
//   vec.emplace_back(dummyMovable());
//   vec.emplace_back(dummyMovable());
//   EXPECT_EQ(vec, dlwhivec);

//   auto dlwhiit =
//       dlwhivec.emplace(dlwhivec.begin() + 1, dummyMovable(), dummyMovable());
//   auto it = vec.emplace(vec.begin() + 1, dummyMovable());
//   vec.emplace(vec.begin() + 1, dummyMovable());
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhiit = dlwhivec.emplace(dlwhivec.begin(), dummyMovable("occupied in thoughts"));
//   it = vec.emplace(vec.begin(), dummyMovable("occupied in thoughts"));
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhiit = dlwhivec.erase(dlwhivec.begin() + 2);
//   it = vec.erase(vec.begin() + 2);
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhiit = dlwhivec.erase(dlwhivec.end() - 1);
//   it = vec.erase(vec.end() - 1);
//   EXPECT_EQ(vec, dlwhivec);
//   EXPECT_EQ(dlwhiit, dlwhivec.end());
// }

// TEST(VectorTest, scenario_insert_many) {
//   dlwhi::vector<dummyMovable> dlwhivec;
//   std::vector<dummyMovable> vec;
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.insert_many_back(dummyMovable());
//   vec.emplace_back(dummyMovable());
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhivec.insert_many_back(dummyMovable(), dummyMovable(), dummyMovable());
//   vec.emplace_back(dummyMovable());
//   vec.emplace_back(dummyMovable());
//   vec.emplace_back(dummyMovable());
//   EXPECT_EQ(vec, dlwhivec);

//   auto dlwhiit =
//       dlwhivec.insert_many(dlwhivec.begin() + 1, dummyMovable(), dummyMovable());
//   auto it = vec.emplace(vec.begin() + 1, dummyMovable());
//   vec.emplace(vec.begin() + 1, dummyMovable());
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhiit =
//       dlwhivec.insert_many(dlwhivec.begin(), dummyMovable("occupied in thoughts"));
//   it = vec.emplace(vec.begin(), dummyMovable("occupied in thoughts"));
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhiit = dlwhivec.erase(dlwhivec.begin() + 2);
//   it = vec.erase(vec.begin() + 2);
//   EXPECT_EQ(*dlwhiit, *it);
//   EXPECT_EQ(vec, dlwhivec);

//   dlwhiit = dlwhivec.erase(dlwhivec.end() - 1);
//   it = vec.erase(vec.end() - 1);
//   EXPECT_EQ(vec, dlwhivec);
//   EXPECT_EQ(dlwhiit, dlwhivec.end());
// }

// TEST(VectorTest, scenario_access_1) {
//   const dlwhi::vector<dummyMovable> dlwhivecConst{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };
//   const std::vector<dummyMovable> vecConst{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };
//   EXPECT_EQ(dlwhivecConst.front(), dummyMovable("Aileen"));
//   EXPECT_EQ(dlwhivecConst.back(), dummyMovable("Grace"));

//   auto it = vecConst.begin();
//   auto dlwhiit = dlwhivecConst.begin();
//   EXPECT_EQ(std::distance(dlwhivecConst.begin(), dlwhivecConst.end()), 5);
//   EXPECT_EQ(std::distance(dlwhivecConst.begin(), dlwhivecConst.end()),
//             std::distance(vecConst.begin(), vecConst.end()));
//   for (; dlwhiit != dlwhivecConst.end(); it++, dlwhiit++) {
//     EXPECT_EQ(*it, *dlwhiit);
//   }

//   EXPECT_EQ(dlwhivecConst.at(1), dummyMovable("Anna"));
//   EXPECT_EQ(dlwhivecConst.at(2), dummyMovable("Louie"));
//   EXPECT_EQ(dlwhivecConst.at(3), dummyMovable("Noel"));

//   auto rit = vecConst.rbegin();
//   auto dlwhirit = dlwhivecConst.rbegin();
//   EXPECT_EQ(std::distance(dlwhivecConst.rbegin(), dlwhivecConst.rend()), 5);
//   EXPECT_EQ(std::distance(dlwhivecConst.rbegin(), dlwhivecConst.rend()),
//             std::distance(vecConst.rbegin(), vecConst.rend()));
//   rit++;
//   dlwhirit++;
//   for (; dlwhirit != dlwhivecConst.rend(); rit++, dlwhirit++) {
//     EXPECT_EQ(*rit, *dlwhirit);
//   }

//   EXPECT_ANY_THROW(dlwhivecConst.at(10));

//   EXPECT_EQ(dlwhivecConst[1], dummyMovable("Anna"));
//   EXPECT_EQ(dlwhivecConst[2], dummyMovable("Louie"));
//   EXPECT_EQ(dlwhivecConst[3], dummyMovable("Noel"));
//   EXPECT_NE(dlwhivecConst.data(), nullptr);
// }

// TEST(VectorTest, scenario_access_2) {
//   dlwhi::vector<dummyMovable> dlwhivec{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };
//   std::vector<dummyMovable> vec{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };

//   EXPECT_EQ(dlwhivec.front(), dummyMovable("Aileen"));
//   EXPECT_EQ(dlwhivec.back(), dummyMovable("Grace"));

//   auto it = vec.begin();
//   auto dlwhiit = dlwhivec.begin();
//   EXPECT_EQ(std::distance(dlwhivec.begin(), dlwhivec.end()), 5);
//   EXPECT_EQ(std::distance(dlwhivec.begin(), dlwhivec.end()),
//             std::distance(vec.begin(), vec.end()));
//   for (; dlwhiit != dlwhivec.end(); it++, dlwhiit++) {
//     EXPECT_EQ(*it, *dlwhiit);
//   }

//   EXPECT_EQ(dlwhivec.at(1), dummyMovable("Anna"));
//   EXPECT_EQ(dlwhivec.at(2), dummyMovable("Louie"));
//   EXPECT_EQ(dlwhivec.at(3), dummyMovable("Noel"));

//   auto rit = vec.rbegin();
//   auto dlwhirit = dlwhivec.rbegin();
//   EXPECT_EQ(std::distance(dlwhivec.rbegin(), dlwhivec.rend()), 5);
//   EXPECT_EQ(std::distance(dlwhivec.rbegin(), dlwhivec.rend()),
//             std::distance(vec.rbegin(), vec.rend()));
//   for (; dlwhirit != dlwhivec.rend(); rit++, dlwhirit++) {
//     EXPECT_EQ(*rit, *dlwhirit);
//   }

//   EXPECT_ANY_THROW(dlwhivec.at(10));

//   EXPECT_EQ(dlwhivec[1], dummyMovable("Anna"));
//   EXPECT_EQ(dlwhivec[2], dummyMovable("Louie"));
//   EXPECT_EQ(dlwhivec[3], dummyMovable("Noel"));

//   dlwhivec.insert(dlwhivec.begin() + 3, dummyMovable("Emil"));
//   EXPECT_EQ(dlwhivec[3], dummyMovable("Emil"));
//   vec.insert(vec.begin() + 3, dummyMovable("Emil"));

//   dlwhivec.erase(dlwhivec.end() - 1);
//   dlwhivec.erase(dlwhivec.end() - 1);
//   vec.erase(vec.end() - 1);
//   vec.erase(vec.end() - 1);
//   EXPECT_EQ(vec, dlwhivec);
// }

// TEST(VectorTest, scenario_ctors) {
//   dlwhi::vector<dummyMovable> vec{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };
//   dlwhi::vector<dummyMovable> cpy(vec);
//   EXPECT_EQ(vec, cpy);
//   dlwhi::vector<dummyMovable> mv(std::move(vec));
//   EXPECT_EQ(mv, cpy);

//   dlwhi::vector<dummyMovable> cpy_ass;
//   cpy_ass = cpy;
//   dlwhi::vector<dummyMovable> mv_ass;
//   mv_ass = std::move(mv);
//   EXPECT_EQ(mv_ass, cpy_ass);
// }

// TEST(VectorTest, scenario_utility) {
//   dlwhi::vector<dummyMovable> vec{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };

//   EXPECT_FALSE(vec.empty());
//   vec.clear();
//   EXPECT_TRUE(vec.empty());

//   EXPECT_NE(vec.data(), nullptr);

//   dlwhi::vector<dummyMovable> other{
//       dummyMovable(),
//       dummyMovable(),
//       dummyMovable(),
//   };

//   vec.swap(other);

//   for (auto i : vec) EXPECT_EQ(i, dummyMovable());
// }

// TEST(VectorTest, scenario_stream) {
//   dlwhi::vector<dummyMovable> vec{
//       dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
//       dummyMovable("Noel"),   dummyMovable("Grace"),
//   };
//   std::stringstream stream;
//   std::string expected("Aileen Anna Louie Noel Grace");
//   stream << vec;
//   EXPECT_EQ(expected, stream.str());
// }
