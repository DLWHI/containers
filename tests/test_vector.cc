#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <list>
#include <random>

#include <dlwhi/vector.h>
#include "test_helpers.h"

// TODO:
//  Add swap tests (but 69 nice)


std::random_device ran_dev;
std::uniform_int_distribution<dlwhi::size_t> uid(1, 1000);
std::mt19937 gen(ran_dev());

#ifdef LOOP_COUNT
constexpr int loop = LOOP_COUNT;
#else
constexpr int loop = 50;
#endif

constexpr int constexpr_check() {
  dlwhi::vector<int> vec = {1, 2, 3, 4, 5};
  vec.emplace(vec.end(), 7);
  vec.reserve(100);
  vec.push_back(6);
  vec.erase(--vec.end());
  vec.insert(vec.begin(), 0); // does not satify constexpr?
  vec.insert(vec.begin() + 1, 0); // satifies constexpr kekwait
  vec.insert(vec.begin() + 3, -1); // satifies constexpr kekwait
  vec.insert(--vec.end(), -1); // satifies constexpr too kekwait
  vec.shrink_to_fit();
  vec.assign({6, 6, 6, 7, 7, 7});
  return *(vec.end() - 1);
}

TEST(VectorTest, ctor_default) {
  dlwhi::vector<dummyCpy> vec;
  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(vec.capacity(), 0);
  EXPECT_EQ(vec.data(), nullptr);
  EXPECT_THROW(vec.at(uid(gen)), std::out_of_range);
}

TEST(VectorTest, ctor_size) {
  dlwhi::size_t random_size = uid(gen);
  std::vector<dummyCpy> vec(random_size);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummyCpy());
  EXPECT_EQ(vec.back(), dummyCpy());
}

TEST(VectorTest, ctor_size_alloc_not_default) {
  dlwhi::size_t random_size = uid(gen);
  std::allocator<dummyCpy> not_default;
  dlwhi::vector<dummyCpy> vec(random_size, not_default);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummyCpy());
  EXPECT_EQ(vec.back(), dummyCpy());
}

TEST(VectorTest, ctor_size_value) {
  dlwhi::size_t random_size = uid(gen);
  std::allocator<dummyCpy> not_default;
  dlwhi::vector<dummyCpy> vec(random_size, dummyCpy("not default"), not_default);

  EXPECT_EQ(vec.size(), random_size);
  EXPECT_EQ(vec.capacity(), random_size);
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), dummyCpy("not default"));
  EXPECT_EQ(vec.back(), dummyCpy("not default"));
}

TEST(VectorTest, ctor_from_stl_vector) {
  dlwhi::size_t random_size = uid(gen);
  std::allocator<dummyCpy> not_default;
  std::vector<dummyCpy> from(random_size, dummyCpy("not default"));

  dlwhi::vector<dummyCpy> vec(from.begin(), from.end(), not_default);
  EXPECT_EQ(vec.size(), from.size());
  EXPECT_EQ(vec.capacity(), from.capacity());
  EXPECT_NE(vec.data(), nullptr);
  EXPECT_EQ(vec.front(), from.front());
  EXPECT_EQ(vec.back(), from.back());
}

TEST(VectorTest, ctor_from_stl_list) {
  dlwhi::size_t random_size = uid(gen);
  std::allocator<dummyCpy> not_default;
  std::list<dummyCpy> from(random_size, dummyCpy("not default"));

  dlwhi::vector<dummyCpy> vec(from.begin(), from.end(), not_default);

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
  dlwhi::vector<dummyCpy> vec1(random_size, dummyCpy("not default"));
  dlwhi::vector<dummyCpy> vec2(vec1);

  EXPECT_EQ(vec1.size(), vec2.size());
  EXPECT_EQ(vec1.capacity(), vec2.capacity());
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec1.front(), vec2.front());
  EXPECT_EQ(vec1.back(), vec2.front());
}

TEST(VectorTest, ctor_move) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummyCpy> vec1(random_size, dummyCpy("not default"));
  dlwhi::vector<dummyCpy> vec2(std::move(vec1));

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummyCpy("not default"));
  EXPECT_EQ(vec2.back(), dummyCpy("not default"));
}

TEST(VectorTest, ctor_move_alloc_not_default) {
  dlwhi::size_t random_size = uid(gen);
  state_allocator<dummyCpy> not_default("not default");
  dlwhi::vector<dummyCpy, state_allocator<dummyCpy>> vec1(random_size, dummyCpy("not default"), state_allocator<dummyCpy>("default"));
  dlwhi::vector<dummyCpy, state_allocator<dummyCpy>> vec2(std::move(vec1), not_default);

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummyCpy("not default"));
  EXPECT_EQ(vec2.back(), dummyCpy("not default"));
  EXPECT_EQ(vec2.get_allocator(), not_default);
}

TEST(VectorTest, ctor_ass_copy) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummyCpy> vec1(random_size, dummyCpy("not default"));
  dlwhi::vector<dummyCpy> vec2;

  vec2 = vec1;

  EXPECT_EQ(vec1.size(), vec2.size());
  EXPECT_EQ(vec1.capacity(), vec2.capacity());
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec1.front(), vec2.front());
  EXPECT_EQ(vec1.back(), vec2.front());
}

TEST(VectorTest, ctor_ass_move) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummyCpy> vec1(random_size, dummyCpy("not default"));
  dlwhi::vector<dummyCpy> vec2;

  vec2 = std::move(vec1);

  EXPECT_EQ(vec2.size(), random_size);
  EXPECT_EQ(vec2.capacity(), random_size);
  EXPECT_NE(vec2.data(), nullptr);
  EXPECT_EQ(vec2.front(), dummyCpy("not default"));
  EXPECT_EQ(vec2.back(), dummyCpy("not default"));
}

TEST(VectorTest, comparison_1) {
  dlwhi::size_t random_size = uid(gen);
  dlwhi::vector<dummyCpy> vec1(random_size, dummyCpy("equal"));
  dlwhi::vector<dummyCpy> vec2(random_size, dummyCpy("equal"));

  EXPECT_TRUE(vec1 == vec2);
  EXPECT_FALSE(vec1 != vec2);
}

TEST(VectorTest, comparison_2) {
  dlwhi::vector<dummyCpy> vec1(uid(gen), dummyCpy("not equal"));
  dlwhi::vector<dummyCpy> vec2(uid(gen), dummyCpy("equal"));

  EXPECT_FALSE(vec1 == vec2);
  EXPECT_TRUE(vec1 != vec2);
}

TEST(VectorTest, comparison_empty) {
  dlwhi::vector<dummyCpy> vec1(uid(gen), dummyCpy("not equal"));
  dlwhi::vector<dummyCpy> vec2;

  EXPECT_FALSE(vec1 == vec2);
  EXPECT_TRUE(vec1 != vec2);
  EXPECT_FALSE(vec2 == vec1);
  EXPECT_TRUE(vec2 != vec1);

  dlwhi::vector<dummyCpy> vec3;
  dlwhi::vector<dummyCpy> vec4;

  EXPECT_TRUE(vec3 == vec4);
  EXPECT_FALSE(vec3 != vec4);
}

TEST(VectorTest, random_access) {
  dlwhi::vector<int> vec(uid(gen));
  for (int i = 0; i < vec.size(); i++) {
    EXPECT_EQ(vec[i], 0);
    vec[i] = i + 1;
  }
  for (int i = 0; i < vec.size(); i++) {
    EXPECT_EQ(vec[i], i + 1);
  }
}

TEST(VectorTest, random_access_bounds) {
  dlwhi::vector<int> vec(uid(gen));
  for (int i = 0; i < vec.size(); i++) {
    EXPECT_EQ(vec.at(i), 0);
    vec.at(i) = i + 1;
  }
  for (int i = 0; i < vec.size(); i++) {
    EXPECT_EQ(vec.at(i), i + 1);
  }
  EXPECT_THROW(vec.at(-1), std::out_of_range);
  EXPECT_THROW(vec.at(vec.size()), std::out_of_range);
}

TEST(VectorTest, front_back_access) {
  dlwhi::vector<int> vec(uid(gen));
  for (int i = 0; i < vec.size(); i++) {
    EXPECT_EQ(vec[i], 0);
    vec[i] = i + 1;
  }
  EXPECT_EQ(vec.front(), 1);
  EXPECT_EQ(vec.back(), vec.size());
}

TEST(VectorTest, assign_1) {
  dlwhi::vector<dummyCpy> vec(uid(gen));

  vec.assign(uid(gen), dummyCpy());

  for (dummyCpy ob: vec) {
    EXPECT_EQ(ob, dummyCpy());
  }
}

TEST(VectorTest, assign_2) {
  dlwhi::vector<dummyCpy> vec1(uid(gen));
  dlwhi::vector<dummyCpy> vec2(uid(gen));

  vec1.assign(uid(gen), dummyCpy());
  vec2.assign(vec1.begin(), vec1.end());

  EXPECT_EQ(vec1, vec2);
}

TEST(VectorTest, assign_3) {
  dlwhi::vector<dummyCpy> vec(uid(gen));

  vec.assign({dummyCpy(), dummyCpy(), dummyCpy(), dummyCpy(), dummyCpy()});

  for (dummyCpy ob: vec) {
    EXPECT_EQ(ob, dummyCpy());
  }
}

TEST(VectorTest, assign_invalid_count) {
  dlwhi::vector<dummyCpy> vec(uid(gen));

  EXPECT_THROW(vec.assign(-uid(gen), dummyCpy()),
               std::length_error);
}

TEST(VectorTest, assign_big_count) {
  dlwhi::vector<dummyCpy> vec(uid(gen));
  std::allocator<dummyCpy> al;
  EXPECT_THROW(vec.assign(std::allocator_traits<std::allocator<dummyCpy>>::max_size(al) + 1, dummyCpy()),
               std::length_error);
}

TEST(VectorTest, assign_invalid_iterators) {
  dlwhi::vector<dummyCpy> vec(uid(gen));

  EXPECT_THROW(vec.assign(vec.end(), vec.begin()),
               std::length_error);
}

TEST(VectorTest, assign_randomly) {
  dlwhi::vector<dummyCpy> vec(uid(gen));

  for (int i = 0; i < loop; i++) {
    vec.assign(uid(gen), dummyCpy());

    for (dummyCpy ob: vec) {
      EXPECT_EQ(ob, dummyCpy());
    }
  }
}

TEST(VectorTest, reserve_expand_not_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyCpy> vec(size);

    size += uid(gen);
    vec.reserve(size);
    EXPECT_EQ(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_shrink_not_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyCpy> vec(size);
    vec.reserve(size / uid(gen));
    EXPECT_EQ(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_invalid_not_movable) {
  dlwhi::vector<dummyCpy> vec(uid(gen));
  EXPECT_ANY_THROW(vec.reserve(-1));
}

TEST(VectorTest, reserve_expand_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMv> vec(size);

    size += uid(gen);
    vec.reserve(size);
    EXPECT_EQ(size, vec.capacity());
    EXPECT_EQ(dummyMv::Constructed::kMove, vec.front().birth);
  }
}

TEST(VectorTest, reserve_shrink_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMv> vec(size);
    vec.reserve(size / uid(gen));
    EXPECT_EQ(size, vec.capacity());
  }
}

TEST(VectorTest, reserve_invalid_movable) {
  dlwhi::vector<dummyMv> vec(uid(gen));
  EXPECT_ANY_THROW(vec.reserve(-1));
}

TEST(VectorTest, stf_not_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyCpy> vec(uid(gen));

    vec.reserve(vec.size() + uid(gen));
    vec.shrink_to_fit();
    EXPECT_EQ(vec.size(), vec.capacity());
  }
}

TEST(VectorTest, stf_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen));

    vec.reserve(vec.size() + uid(gen));
    vec.shrink_to_fit();
    EXPECT_EQ(vec.size(), vec.capacity());
    EXPECT_EQ(dummyMv::Constructed::kMove, vec.back().birth);
    EXPECT_EQ(dummyMv::Constructed::kMove, vec.front().birth);
  }
}

TEST(VectorTest, clear) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("dirty"));

    vec.clear();
    EXPECT_EQ(vec.size(), 0);
  }
}

TEST(VectorTest, resize_not_movable) {
  dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    vec.resize(size, dummyCpy("appended"));
    EXPECT_EQ(size, vec.size());
  }
}

TEST(VectorTest, resize_expand_not_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyCpy> vec(size, dummyCpy("default"));

    size += uid(gen);
    vec.resize(size, dummyCpy("appended"));
    EXPECT_EQ(size, vec.capacity());
    EXPECT_EQ(dummyCpy("appended"), vec.back());
  }
}

TEST(VectorTest, resize_shrink_not_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyCpy> vec(size);

    size = 1 + size / (1 + uid(gen));
    vec.resize(size);
    EXPECT_LE(size, vec.capacity());
  }
}

TEST(VectorTest, resize_zero_not_movable) {
  dlwhi::vector<dummyCpy> vec(uid(gen));

  vec.resize(0);
  EXPECT_EQ(0, vec.size());
}

TEST(VectorTest, resize_invalid_not_movable) {
  dlwhi::vector<dummyCpy> vec(uid(gen));
  EXPECT_THROW(vec.resize(-1, dummyCpy("dolbaeb")),
               std::length_error);
}

TEST(VectorTest, resize_movable) {
  dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    vec.resize(size);
    EXPECT_EQ(size, vec.size());
  }
}

TEST(VectorTest, resize_expand_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMv> vec(size, dummyMv("default"));

    size += uid(gen);
    vec.resize(size);
    EXPECT_EQ(size, vec.capacity());
    EXPECT_EQ(dummyMv(), vec.back());
    EXPECT_EQ(dummyMv::Constructed::kMove, vec.front().birth);
  }
}

TEST(VectorTest, resize_shrink_movable) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMv> vec(size);

    size = 1 + size / (1 + uid(gen));
    vec.resize(size, dummyMv("trick"));
    EXPECT_LE(size, vec.capacity());
    EXPECT_NE(dummyMv("trick"), vec.back());
  }
}

TEST(VectorTest, resize_zero_movable) {
  dlwhi::vector<dummyMv> vec(uid(gen));

  vec.resize(0);
  EXPECT_EQ(0, vec.size());
}

TEST(VectorTest, resize_invalid_movable) {
  dlwhi::vector<dummyMv> vec(uid(gen));
  EXPECT_THROW(vec.resize(-1, dummyMv("you knew it")),
               std::length_error);
}

TEST(VectorTest, insert_not_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dummyCpy insert_val("inserted");
  dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
  std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
  for (int i = 0; i < insert_count; i++) {
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.insert(insert_pos, insert_val);
    EXPECT_EQ(*insert_pos, insert_val);
  }
}

TEST(VectorTest, insert_not_movable_2) {
  dummyCpy insert_val("inserted");
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.insert(insert_pos, insert_val);
    EXPECT_EQ(*insert_pos, insert_val);
  }
}

TEST(VectorTest, insert_empty_not_movable) {
  dummyMv insert_val("inserted");
  dlwhi::vector<dummyCpy> vec;
  auto insert_pos = vec.insert(vec.begin(), insert_val);
  EXPECT_EQ(insert_pos, vec.begin());
}

TEST(VectorTest, insert_begin_not_movable) {
  dummyCpy target("inserted");
  dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
  auto insert_pos = vec.insert(vec.begin(), target);
  EXPECT_EQ(*insert_pos, target);
  EXPECT_EQ(*vec.begin(), target);
  EXPECT_EQ(vec.begin(), insert_pos);
}

TEST(VectorTest, insert_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
  std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
  for (int i = 0; i < insert_count; i++) {
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.insert(insert_pos, dummyMv("inserted"));
    EXPECT_EQ(*insert_pos, dummyMv("inserted"));
  }
}

TEST(VectorTest, insert_movable_2) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.insert(insert_pos, dummyMv("inserted"));
    EXPECT_EQ(*insert_pos, dummyMv("inserted"));
  }
}

TEST(VectorTest, insert_empty_movable) {
  dlwhi::vector<dummyMv> vec;
  auto insert_pos = vec.insert(vec.begin(), dummyMv("inserted"));
  EXPECT_EQ(insert_pos, vec.begin());
}

TEST(VectorTest, insert_begin_movable) {
  dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
  auto insert_pos = vec.insert(vec.begin(), dummyMv("inserted"));
  EXPECT_EQ(*insert_pos, dummyMv("inserted"));
  EXPECT_EQ(*vec.begin(), dummyMv("inserted"));
  EXPECT_EQ(vec.begin(), insert_pos);
  EXPECT_EQ(vec.back().birth, dummyMv::Constructed::kMove);
}

TEST(VectorTest, push_back_not_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dummyCpy push_value("pushed");
  dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
  for (int i = 0; i < insert_count; i++) {
    vec.push_back(push_value);
    EXPECT_EQ(vec.back(), push_value);
  }
}

TEST(VectorTest, push_back_not_movable_2) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dummyCpy push_value("pushed");
  for (int i = 0; i < insert_count; i++) {
    dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
    vec.push_back(push_value);
    EXPECT_EQ(vec.back(), push_value);
  }
}

TEST(VectorTest, push_back_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
  for (int i = 0; i < insert_count; i++) {
    vec.push_back(dummyMv("pushed"));
    EXPECT_EQ(vec.back(), dummyMv("pushed"));
  }
}

TEST(VectorTest, push_back_movable_2) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  for (int i = 0; i < insert_count; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
    vec.push_back(dummyMv("pushed"));
    EXPECT_EQ(vec.back(), dummyMv("pushed"));
  }
}

TEST(VectorTest, erase_1) {
  int erase_count = std::min(static_cast<int>(uid(gen)), loop);
  dlwhi::size_t size = std::max(static_cast<int>(uid(gen)), erase_count);
  dlwhi::vector<dummyCpy> vec(size, dummyCpy("default"));
  for (int i = 0; i < erase_count; i++) {
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size() - 1);
    auto pos = vec.begin() + uid_vec(gen);
    vec.erase(pos);
  }
  EXPECT_EQ(vec.size(), size - erase_count);
}

TEST(VectorTest, erase_2) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size() - 1);
    auto pos = vec.begin() + uid_vec(gen);
    pos = vec.insert(pos + 1, dummyMv("inserted"));
    pos = vec.erase(pos - 1);
    EXPECT_EQ(*pos, dummyMv("inserted"));
  }
}

TEST(VectorTest, erase_empty) {
  dlwhi::size_t size = uid(gen);
  dlwhi::vector<dummyCpy> vec(size);
  for (int i = 0; i < size; i++) {
    vec.erase(vec.begin());
  }
  EXPECT_EQ(vec.size(), 0);
}

TEST(VectorTest, pop_back_1) {
  int erase_count = std::min(static_cast<int>(uid(gen)), loop);
  dlwhi::size_t size = std::max(static_cast<int>(uid(gen)), erase_count);
  dlwhi::vector<dummyCpy> vec(size, dummyCpy("default"));
  for (int i = 0; i < erase_count; i++) {
    vec.pop_back();
  }
  EXPECT_EQ(vec.size(), size - erase_count);
}

TEST(VectorTest, pop_back_2) {
  for (int i = 0; i < loop; i++) {
    dlwhi::size_t size = uid(gen);
    dlwhi::vector<dummyMv> vec(size, dummyMv("default"));
    vec.pop_back();
    EXPECT_EQ(vec.size(), size - 1);
  }
}

TEST(VectorTest, pop_empty) {
  dlwhi::size_t size = uid(gen);
  dlwhi::vector<dummyCpy> vec(size);
  for (int i = 0; i < size; i++) {
    vec.pop_back();
  }
  EXPECT_EQ(vec.size(), 0);
}

TEST(VectorTest, emplace_not_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
  std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
  for (int i = 0; i < insert_count; i++) {
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.emplace(insert_pos, "inserted");
    EXPECT_EQ(*insert_pos, dummyCpy("inserted"));
  }
}

TEST(VectorTest, emplace_not_movable_2) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyCpy> vec(uid(gen), dummyCpy("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.emplace(insert_pos, "inserted");
    EXPECT_EQ(*insert_pos, dummyCpy("inserted"));
  }
}

TEST(VectorTest, emplace_movable_1) {
  int insert_count = std::min(static_cast<int>(uid(gen)), loop);
  dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
  std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
  for (int i = 0; i < insert_count; i++) {
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.emplace(insert_pos, "inserted");
    EXPECT_EQ(*insert_pos, dummyMv("inserted"));
    EXPECT_EQ(dummyMv::Constructed::kParam, insert_pos->birth);
  }
}

TEST(VectorTest, emplace_movable_2) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(0, vec.size());
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.emplace(insert_pos, "inserted");
    EXPECT_EQ(*insert_pos, dummyMv("inserted"));
    EXPECT_EQ(dummyMv::Constructed::kParam, insert_pos->birth);
  }
}

TEST(VectorTest, emplace_from_begin) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
    std::uniform_int_distribution<dlwhi::size_t> uid_vec(1, vec.size());
    auto insert_pos = vec.begin() + uid_vec(gen);
    insert_pos = vec.emplace(insert_pos, std::move(vec.front()));
    EXPECT_NE(vec.front(), dummyMv("default"));
    EXPECT_EQ(dummyMv::Constructed::kMove, insert_pos->birth);
  }
}

TEST(VectorTest, emplace_back_from_begin) {
  for (int i = 0; i < loop; i++) {
    dlwhi::vector<dummyMv> vec(uid(gen), dummyMv("default"));
    vec.emplace_back(std::move(vec.front()));
    EXPECT_EQ(vec.back(), dummyMv("default"));
    EXPECT_EQ(vec.back().birth, dummyMv::Constructed::kMove);
    EXPECT_NE(vec.front(), dummyMv("default"));
  }
}

TEST(VectorTest, emplace_back_vector) {
  dlwhi::size_t size = uid(gen);
  dlwhi::vector<dummyMv> vec(size, dummyMv("default"));
  for (dlwhi::size_t i = 0; i < size; i++) {
    vec.emplace_back(std::move(vec[i]));
  }
  EXPECT_EQ(vec.size(), size*2);
  for (dlwhi::size_t i = 0; i < size; i++) {
    EXPECT_NE(vec[i], dummyMv("default"));
  }
  for (dlwhi::size_t i = size; i < 2*size; i++) {
    EXPECT_EQ(vec[i], dummyMv("default"));
    EXPECT_EQ(vec.back().birth, dummyMv::Constructed::kMove);
  }
}

TEST(VectorTest, stream) {
  dlwhi::vector<dummyMv> vec{
      dummyMv("Aileen"), dummyMv("Anna"),  dummyMv("Louie"),
      dummyMv("Noel"),   dummyMv("Grace"),
  };
  std::stringstream stream;
  std::string expected("Aileen Anna Louie Noel Grace");
  stream << vec;
  EXPECT_EQ(expected, stream.str());
}

TEST(VectorTest, valid_constexpr) {
  constexpr int cexper = constexpr_check();
  EXPECT_EQ(cexper, 7);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
