#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <list>
#include <random>

#include <dlwhi/vector.h>
#include "test_helpers.h"

std::random_device ran_dev;
std::uniform_int_distribution<dlwhi::size_t> uid(1, 1000);
std::mt19937 gen(ran_dev());


TEST(VectorTest, ctor_default) {
  dlwhi::vector<dummy> vec;
  std::vector<dummy> s;
  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(vec.capacity(), 0);
  EXPECT_EQ(vec.data(), nullptr);
  EXPECT_THROW(vec.at(uid(gen)), std::out_of_range);
}

TEST(VectorTest, ctor_size) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec(random_size);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummy());
  EXPECT_EQ(vec.back(), dummy());
}

TEST(VectorTest, ctor_size_alloc_not_default) {
  dlwhi::size_t random_size = uid(gen);
  std::allocator<dummy> not_default;
  dlwhi::vector<dummy> vec(random_size, not_default);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummy());
  EXPECT_EQ(vec.back(), dummy());
}

TEST(VectorTest, ctor_size_value) {
  dlwhi::size_t random_size = uid(gen);
  std::allocator<dummy> not_default;
  dlwhi::vector<dummy> vec(random_size, dummy("not default"), not_default);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummy("not default"));
  EXPECT_EQ(vec.back(), dummy("not default"));
}

TEST(VectorTest, ctor_from_stl_vector) {
  dlwhi::size_t random_size = uid(gen);
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
  dlwhi::size_t random_size = uid(gen);
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
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2(vec1);

  EXPECT_EQ(vec1.size(), vec2.size());
  EXPECT_EQ(vec1.capacity(), vec2.capacity());
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec1.front(), vec2.front());
  EXPECT_EQ(vec1.back(), vec2.front());
}

TEST(VectorTest, ctor_move) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2(std::move(vec1));

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummy("not default"));
  EXPECT_EQ(vec2.back(), dummy("not default"));
}

TEST(VectorTest, ctor_move_alloc_not_default) {
  dlwhi::size_t random_size = uid(gen);
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
  dlwhi::size_t random_size = uid(gen);
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
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("not default"));
  dlwhi::vector<dummy> vec2;

  vec2 = std::move(vec1);

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummy("not default"));
  EXPECT_EQ(vec2.back(), dummy("not default"));
}

TEST(VectorTest, comparison_1) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummy> vec1(random_size, dummy("equal"));
  dlwhi::vector<dummy> vec2(random_size, dummy("equal"));

  EXPECT_TRUE(vec1 == vec2);
  EXPECT_FALSE(vec1 != vec2);
}

TEST(VectorTest, comparison_2) {
  dlwhi::vector<dummy> vec1(uid(gen), dummy("not equal"));
  dlwhi::vector<dummy> vec2(uid(gen), dummy("equal"));

  EXPECT_FALSE(vec1 == vec2);
  EXPECT_TRUE(vec1 != vec2);
}

TEST(VectorTest, comparison_empty) {
  dlwhi::vector<dummy> vec1(uid(gen), dummy("not equal"));
  dlwhi::vector<dummy> vec2;

  EXPECT_FALSE(vec1 == vec2);
  EXPECT_TRUE(vec1 != vec2);
  EXPECT_FALSE(vec2 == vec1);
  EXPECT_TRUE(vec2 != vec1);

  dlwhi::vector<dummy> vec3;
  dlwhi::vector<dummy> vec4;

  EXPECT_TRUE(vec3 == vec4);
  EXPECT_FALSE(vec3 != vec4);
}

TEST(VectorTest, assign_1) {
  dlwhi::vector<dummy> vec(uid(gen));

  vec.assign(uid(gen), dummy());
  
  for (dummy ob: vec) {
    EXPECT_EQ(ob, dummy());
  }
}

TEST(VectorTest, assign_2) {
  dlwhi::vector<dummy> vec1(uid(gen));
  dlwhi::vector<dummy> vec2(uid(gen));

  vec1.assign(uid(gen), dummy());
  vec2.assign(vec1.begin(), vec1.end());
  
  EXPECT_EQ(vec1, vec2);
}

TEST(VectorTest, assign_3) {
  dlwhi::vector<dummy> vec(uid(gen));

  vec.assign({dummy(), dummy(), dummy(), dummy(), dummy()});
  
  for (dummy ob: vec) {
    EXPECT_EQ(ob, dummy());
  }
}

TEST(VectorTest, assign_invalid_count) {
  dlwhi::vector<dummy> vec(uid(gen));

  EXPECT_THROW(vec.assign(-uid(gen), dummy()),
               std::length_error);
}

TEST(VectorTest, assign_big_count) {
  dlwhi::vector<dummy> vec(uid(gen));
  std::allocator<dummy> al;
  EXPECT_THROW(vec.assign(std::allocator_traits<std::allocator<dummy>>::max_size(al) + 1, dummy()),
               std::length_error);
}

TEST(VectorTest, assign_invalid_iterators) {
  dlwhi::vector<dummy> vec(uid(gen));

  EXPECT_THROW(vec.assign(vec.end(), vec.begin()),
               std::length_error);
}

TEST(VectorTest, assign_randomly) {
  dlwhi::vector<dummy> vec(uid(gen));

  for (int i = 0; i < 15; i++) {
    vec.assign(uid(gen), dummy());
    
    for (dummy ob: vec) {
      EXPECT_EQ(ob, dummy());
    }
  }
}

TEST(VectorTest, reserve_expand) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMovable> vec(size);

    size += uid(gen);
    vec.reserve(size);
    EXPECT_EQ(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_shrink) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMovable> vec(size);

    size /= uid(gen) + 1;
    vec.reserve(size);
    EXPECT_LT(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_invalid) {
  dlwhi::vector<dummyMovable> vec(uid(gen));
  EXPECT_ANY_THROW(vec.reserve(-1));
}

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

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
