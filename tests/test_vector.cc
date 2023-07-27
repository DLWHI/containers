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

TEST(VectorTest, reserve_expand_not_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummy> vec(size);

    size += uid(gen);
    vec.reserve(size);
    EXPECT_EQ(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_shrink_not_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummy> vec(size);

    size /= uid(gen) + 1;
    vec.reserve(size);
    EXPECT_LT(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_invalid_not_movable) {
  dlwhi::vector<dummy> vec(uid(gen));
  EXPECT_ANY_THROW(vec.reserve(-1));
}

TEST(VectorTest, reserve_expand_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMovable> vec(size);

    size += uid(gen);
    vec.reserve(size);
    EXPECT_EQ(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_shrink_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMovable> vec(size);

    size /= uid(gen) + 1;
    vec.reserve(size);
    EXPECT_LT(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_invalid_movable) {
  dlwhi::vector<dummyMovable> vec(uid(gen));
  EXPECT_ANY_THROW(vec.reserve(-1));
}

TEST(VectorTest, stf_not_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::vector<dummy> vec(uid(gen));

    vec.reserve(vec.size() + uid(gen));
    vec.shrink_to_fit();
    EXPECT_EQ(vec.size(), vec.capacity());
  }
}

TEST(VectorTest, stf_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::vector<dummyMovable> vec(uid(gen));

    vec.reserve(vec.size() + uid(gen));
    vec.shrink_to_fit();
    EXPECT_EQ(vec.size(), vec.capacity());
  }
}

TEST(VectorTest, clear) {
  for (int i = 0; i < 15; i++) {
    dlwhi::vector<dummy> vec(uid(gen), dummy("dirty"));

    vec.clear();
    EXPECT_EQ(vec.size(), 0);
  }
}

TEST(VectorTest, resize_expand_not_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummy> vec(size, dummy("default"));

    size += uid(gen);
    vec.resize(size, dummy("appended"));
    EXPECT_EQ(size, vec.capacity());
    EXPECT_EQ(dummy("appended"), vec.back());
  }
}

TEST(VectorTest, resize_shrink_not_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummy> vec(size);

    size /= uid(gen) + 1;
    vec.resize(size);
    EXPECT_LT(size, vec.capacity());
  }
}

TEST(VectorTest, resize_invalid_not_movable) {
  dlwhi::vector<dummy> vec(uid(gen));
  EXPECT_THROW(vec.resize(-1, dummy("dolbaeb")),
               std::length_error);
}

TEST(VectorTest, resize_expand_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMovable> vec(size, dummyMovable("default"));

    size += uid(gen);
    vec.resize(size);
    EXPECT_EQ(size, vec.capacity());
    EXPECT_EQ(dummyMovable(), vec.back());
  }
}

TEST(VectorTest, resize_shrink_movable) {
  for (int i = 0; i < 15; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMovable> vec(size);

    size /= uid(gen) + 1;
    vec.resize(size, dummyMovable("trick"));
    EXPECT_LT(size, vec.capacity());
    EXPECT_NE(dummyMovable("trick"), vec.back());
  }
}

TEST(VectorTest, resize_invalid_movable) {
  dlwhi::vector<dummyMovable> vec(uid(gen));
  EXPECT_THROW(vec.resize(-1, dummyMovable("you knew it")),
               std::length_error);
}

TEST(VectorTest, insert_not_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), 15);
  dummy insert_val("inserted");
  dlwhi::vector<dummy> vec(uid(gen), dummy("default"));
  std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
  for (int i = 0; i < insert_count; i++) {
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.insert(insert_pos, insert_val);
    EXPECT_EQ(*insert_pos, insert_val);
  }
}

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

TEST(VectorTest, stream) {
  dlwhi::vector<dummyMovable> vec{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };
  std::stringstream stream;
  std::string expected("Aileen Anna Louie Noel Grace");
  stream << vec;
  EXPECT_EQ(expected, stream.str());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
