// #include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include "../containers/s21_vector.h"
#include "dummy.h"

template <typename T>
bool operator==(const std::vector<T>& vec, const s21::vector<T>& s21vec) {
  if (s21vec.size() != vec.size() || s21vec.capacity() != vec.capacity())
    return false;
  for (size_t i = 0; i < s21vec.size(); i++)
    if (!(vec[i] == s21vec[i])) return false;
  return true;
}

template <typename T>
bool operator==(const s21::vector<T>& s21vec, const std::vector<T>& vec) {
  if (s21vec.size() != vec.size() || s21vec.capacity() != vec.capacity())
    return false;
  for (size_t i = 0; i < s21vec.size(); i++)
    if (!(vec[i] == s21vec[i])) return false;
  return true;
}

TEST(VectorTest, scenario_1) {
  s21::vector<dummy> s21vec;
  std::vector<dummy> vec;
  EXPECT_EQ(s21vec, vec);

  dummy one("one");
  s21vec.push_back(one);
  vec.push_back(one);
  EXPECT_TRUE(s21vec == vec);

  dummy two("two");
  s21vec.push_back(two);
  vec.push_back(two);
  EXPECT_TRUE(s21vec == vec);

  dummy three("three");
  dummy four("four");
  dummy five("five");
  s21vec.push_back(three);
  s21vec.push_back(four);
  s21vec.push_back(five);
  vec.push_back(three);
  vec.push_back(four);
  vec.push_back(five);
  EXPECT_TRUE(s21vec == vec);

  s21vec.pop_back();
  vec.pop_back();
  EXPECT_TRUE(s21vec == vec);

  dummy six("six");
  dummy seven("seven");
  s21vec.push_back(six);
  vec.push_back(six);
  s21vec.push_back(seven);
  vec.push_back(seven);
  EXPECT_TRUE(s21vec == vec);

  s21vec.pop_back();
  s21vec.pop_back();
  vec.pop_back();
  vec.pop_back();
  EXPECT_TRUE(s21vec == vec);

  s21vec.clear();
  vec.clear();
  EXPECT_TRUE(s21vec == vec);
}

TEST(VectorTest, scenario_2) {
  s21::vector<dummy> s21vec(12);
  std::vector<dummy> vec(12);
  EXPECT_EQ(s21vec, vec);

  dummy one("one");
  s21vec.push_back(one);
  vec.push_back(one);
  EXPECT_TRUE(s21vec == vec);

  dummy two("two");
  s21vec.push_back(two);
  vec.push_back(two);
  EXPECT_TRUE(s21vec == vec);

  dummy three("three");
  dummy four("four");
  dummy five("five");
  s21vec.push_back(three);
  s21vec.push_back(four);
  s21vec.push_back(five);
  vec.push_back(three);
  vec.push_back(four);
  vec.push_back(five);
  EXPECT_TRUE(s21vec == vec);

  s21vec.pop_back();
  vec.pop_back();
  EXPECT_TRUE(s21vec == vec);

  dummy six("six");
  dummy seven("seven");
  s21vec.push_back(six);
  vec.push_back(six);
  s21vec.push_back(seven);
  vec.push_back(seven);
  EXPECT_TRUE(s21vec == vec);

  s21vec.shrink_to_fit();
  vec.shrink_to_fit();
  EXPECT_TRUE(s21vec == vec);
}

TEST(VectorTest, scenario_3) {
  s21::vector<dummyMovable> s21vec(12, dummyMovable("was there"));
  std::vector<dummyMovable> vec(12, dummyMovable("was there"));
  EXPECT_EQ(s21vec, vec);

  s21vec.push_back(dummyMovable("pushed1"));
  vec.push_back(dummyMovable("pushed1"));
  EXPECT_TRUE(s21vec == vec);

  s21vec.push_back(dummyMovable("pushed2"));
  vec.push_back(dummyMovable("pushed2"));
  EXPECT_TRUE(s21vec == vec);

  s21vec.push_back(dummyMovable("pushed3"));
  vec.push_back(dummyMovable("pushed3"));
  s21vec.push_back(dummyMovable("pushed4"));
  vec.push_back(dummyMovable("pushed4"));
  s21vec.push_back(dummyMovable("pushed5"));
  vec.push_back(dummyMovable("pushed5"));
  EXPECT_TRUE(s21vec == vec);

  s21vec.pop_back();
  vec.pop_back();
  EXPECT_TRUE(s21vec == vec);

  dummy six("six");
  dummy seven("seven");
  s21vec.push_back(dummyMovable("pushed6"));
  vec.push_back(dummyMovable("pushed6"));
  s21vec.push_back(dummyMovable("pushed7"));
  vec.push_back(dummyMovable("pushed7"));
  EXPECT_TRUE(s21vec == vec);

  s21vec.reserve(14);
  s21vec.reserve(14);
  EXPECT_TRUE(s21vec == vec);
}

TEST(VectorTest, scenario_assign_1) {
  s21::vector<dummyMovable> s21vec(3);
  std::vector<dummyMovable> vec(3);
  EXPECT_EQ(vec, s21vec);

  vec.assign({dummyMovable(), dummyMovable(), dummyMovable(), dummyMovable(),
              dummyMovable()});
  s21vec.assign({dummyMovable(), dummyMovable(), dummyMovable(), dummyMovable(),
                 dummyMovable()});
  EXPECT_EQ(vec, s21vec);

  dummyMovable target("what");
  s21vec.assign(17, target);
  vec.assign(17, target);
  EXPECT_EQ(vec, s21vec);

  s21vec.clear();
  EXPECT_FALSE(vec == s21vec);
  s21vec.assign(vec.begin(), vec.end());
  EXPECT_EQ(vec, s21vec);

  s21vec.clear();
  EXPECT_ANY_THROW(s21vec.assign(vec.end(), vec.begin()));
}

TEST(VectorTest, scenario_assign_2) {
  s21::vector<dummy> s21vec(12);
  std::vector<dummy> vec(12);
  EXPECT_EQ(vec, s21vec);
  dummy target("what");
  s21vec.assign(5, target);
  vec.assign(5, target);
  EXPECT_EQ(vec, s21vec);

  s21vec.clear();
  vec.clear();
  EXPECT_EQ(vec, s21vec);
  s21vec.assign(2, target);
  vec.assign(2, target);
  EXPECT_EQ(vec, s21vec);

  s21vec.resize(20);
  vec.resize(20);
  EXPECT_EQ(vec, s21vec);

  s21vec.assign(12, target);
  vec.assign(12, target);
  EXPECT_EQ(vec, s21vec);

  EXPECT_ANY_THROW(s21vec.assign(-1, target));
}

TEST(VectorTest, scenario_insert) {
  s21::vector<dummyMovable> s21vec(12);
  std::vector<dummyMovable> vec(12);

  dummyMovable target("what");
  auto s21it = s21vec.begin() + 4;
  auto it = vec.begin() + 4;

  EXPECT_EQ(*s21it, *it);
  it++;
  s21it++;
  EXPECT_EQ(*s21it, *it);

  it = vec.insert(it, target);
  s21it = s21vec.insert(s21it, target);
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(s21vec, vec);

  it = vec.insert(it, dummyMovable("inserted"));
  s21it = s21vec.insert(s21it, dummyMovable("inserted"));
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(s21vec, vec);

  it = vec.insert(vec.begin() + 7, dummyMovable("second inserted"));
  s21it = s21vec.insert(s21vec.begin() + 7, dummyMovable("second inserted"));
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(s21vec, vec);

  it = vec.erase(it);
  s21it = s21vec.erase(s21it);
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(s21vec, vec);
}

TEST(VectorTest, scenario_resize) {
  s21::vector<dummyMovable> s21vec{
      dummyMovable("not default"), dummyMovable("not default"),
      dummyMovable("not default"), dummyMovable("not default"),
      dummyMovable("not default")};
  std::vector<dummyMovable> vec{
      dummyMovable("not default"), dummyMovable("not default"),
      dummyMovable("not default"), dummyMovable("not default"),
      dummyMovable("not default")};

  s21vec.resize(33);
  vec.resize(33);
  EXPECT_EQ(vec, s21vec);

  s21vec.resize(17);
  vec.resize(17);
  EXPECT_EQ(vec, s21vec);

  s21vec.resize(3);
  vec.resize(3);
  EXPECT_EQ(vec, s21vec);

  s21vec.resize(7, dummyMovable("nd"));
  vec.resize(7, dummyMovable("nd"));
  EXPECT_EQ(vec, s21vec);

  s21vec.resize(7);
  vec.resize(7);
  EXPECT_EQ(vec, s21vec);

  s21vec.resize(8, dummyMovable("that one is mine"));
  vec.resize(8, dummyMovable("that one is mine"));
  EXPECT_EQ(vec, s21vec);

  EXPECT_ANY_THROW(s21vec.resize(-1));
}

TEST(VectorTest, scenario_reserve) {
  dummyMovable target("this");
  s21::vector<dummyMovable> s21vec(12, target);
  std::vector<dummyMovable> vec(12, target);

  s21vec.reserve(6);
  vec.reserve(6);
  EXPECT_EQ(s21vec, vec);

  s21vec.reserve(15);
  vec.reserve(15);
  EXPECT_EQ(s21vec, vec);

  s21vec.reserve(0);
  vec.reserve(0);
  EXPECT_EQ(s21vec, vec);

  EXPECT_ANY_THROW(s21vec.reserve(-1));
}

TEST(VectorTest, scenario_emplace) {
  s21::vector<dummyMovable> s21vec;
  std::vector<dummyMovable> vec;
  EXPECT_EQ(vec, s21vec);

  s21vec.emplace_back(dummyMovable());
  vec.emplace_back(dummyMovable());
  EXPECT_EQ(vec, s21vec);

  s21vec.emplace_back(dummyMovable(), dummyMovable(), dummyMovable());
  vec.emplace_back(dummyMovable());
  vec.emplace_back(dummyMovable());
  vec.emplace_back(dummyMovable());
  EXPECT_EQ(vec, s21vec);

  auto s21it =
      s21vec.emplace(s21vec.begin() + 1, dummyMovable(), dummyMovable());
  auto it = vec.emplace(vec.begin() + 1, dummyMovable());
  vec.emplace(vec.begin() + 1, dummyMovable());
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(vec, s21vec);

  s21it = s21vec.emplace(s21vec.begin(), dummyMovable("occupied in thoughts"));
  it = vec.emplace(vec.begin(), dummyMovable("occupied in thoughts"));
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(vec, s21vec);

  s21it = s21vec.erase(s21vec.begin() + 2);
  it = vec.erase(vec.begin() + 2);
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(vec, s21vec);

  s21it = s21vec.erase(s21vec.end() - 1);
  it = vec.erase(vec.end() - 1);
  EXPECT_EQ(vec, s21vec);
  EXPECT_EQ(s21it, s21vec.end());
}

TEST(VectorTest, scenario_insert_many) {
  s21::vector<dummyMovable> s21vec;
  std::vector<dummyMovable> vec;
  EXPECT_EQ(vec, s21vec);

  s21vec.insert_many_back(dummyMovable());
  vec.emplace_back(dummyMovable());
  EXPECT_EQ(vec, s21vec);

  s21vec.insert_many_back(dummyMovable(), dummyMovable(), dummyMovable());
  vec.emplace_back(dummyMovable());
  vec.emplace_back(dummyMovable());
  vec.emplace_back(dummyMovable());
  EXPECT_EQ(vec, s21vec);

  auto s21it =
      s21vec.insert_many(s21vec.begin() + 1, dummyMovable(), dummyMovable());
  auto it = vec.emplace(vec.begin() + 1, dummyMovable());
  vec.emplace(vec.begin() + 1, dummyMovable());
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(vec, s21vec);

  s21it =
      s21vec.insert_many(s21vec.begin(), dummyMovable("occupied in thoughts"));
  it = vec.emplace(vec.begin(), dummyMovable("occupied in thoughts"));
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(vec, s21vec);

  s21it = s21vec.erase(s21vec.begin() + 2);
  it = vec.erase(vec.begin() + 2);
  EXPECT_EQ(*s21it, *it);
  EXPECT_EQ(vec, s21vec);

  s21it = s21vec.erase(s21vec.end() - 1);
  it = vec.erase(vec.end() - 1);
  EXPECT_EQ(vec, s21vec);
  EXPECT_EQ(s21it, s21vec.end());
}

TEST(VectorTest, scenario_access_1) {
  const s21::vector<dummyMovable> s21vecConst{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };
  const std::vector<dummyMovable> vecConst{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };
  EXPECT_EQ(s21vecConst.front(), dummyMovable("Aileen"));
  EXPECT_EQ(s21vecConst.back(), dummyMovable("Grace"));

  auto it = vecConst.begin();
  auto s21it = s21vecConst.begin();
  EXPECT_EQ(std::distance(s21vecConst.begin(), s21vecConst.end()), 5);
  EXPECT_EQ(std::distance(s21vecConst.begin(), s21vecConst.end()),
            std::distance(vecConst.begin(), vecConst.end()));
  for (; s21it != s21vecConst.end(); it++, s21it++) {
    EXPECT_EQ(*it, *s21it);
  }

  EXPECT_EQ(s21vecConst.at(1), dummyMovable("Anna"));
  EXPECT_EQ(s21vecConst.at(2), dummyMovable("Louie"));
  EXPECT_EQ(s21vecConst.at(3), dummyMovable("Noel"));

  auto rit = vecConst.rbegin();
  auto s21rit = s21vecConst.rbegin();
  EXPECT_EQ(std::distance(s21vecConst.rbegin(), s21vecConst.rend()), 5);
  EXPECT_EQ(std::distance(s21vecConst.rbegin(), s21vecConst.rend()),
            std::distance(vecConst.rbegin(), vecConst.rend()));
  rit++;
  s21rit++;
  for (; s21rit != s21vecConst.rend(); rit++, s21rit++) {
    EXPECT_EQ(*rit, *s21rit);
  }

  EXPECT_ANY_THROW(s21vecConst.at(10));

  EXPECT_EQ(s21vecConst[1], dummyMovable("Anna"));
  EXPECT_EQ(s21vecConst[2], dummyMovable("Louie"));
  EXPECT_EQ(s21vecConst[3], dummyMovable("Noel"));
  EXPECT_NE(s21vecConst.data(), nullptr);
}

TEST(VectorTest, scenario_access_2) {
  s21::vector<dummyMovable> s21vec{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };
  std::vector<dummyMovable> vec{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };

  EXPECT_EQ(s21vec.front(), dummyMovable("Aileen"));
  EXPECT_EQ(s21vec.back(), dummyMovable("Grace"));

  auto it = vec.begin();
  auto s21it = s21vec.begin();
  EXPECT_EQ(std::distance(s21vec.begin(), s21vec.end()), 5);
  EXPECT_EQ(std::distance(s21vec.begin(), s21vec.end()),
            std::distance(vec.begin(), vec.end()));
  for (; s21it != s21vec.end(); it++, s21it++) {
    EXPECT_EQ(*it, *s21it);
  }

  EXPECT_EQ(s21vec.at(1), dummyMovable("Anna"));
  EXPECT_EQ(s21vec.at(2), dummyMovable("Louie"));
  EXPECT_EQ(s21vec.at(3), dummyMovable("Noel"));

  auto rit = vec.rbegin();
  auto s21rit = s21vec.rbegin();
  EXPECT_EQ(std::distance(s21vec.rbegin(), s21vec.rend()), 5);
  EXPECT_EQ(std::distance(s21vec.rbegin(), s21vec.rend()),
            std::distance(vec.rbegin(), vec.rend()));
  for (; s21rit != s21vec.rend(); rit++, s21rit++) {
    EXPECT_EQ(*rit, *s21rit);
  }

  EXPECT_ANY_THROW(s21vec.at(10));

  EXPECT_EQ(s21vec[1], dummyMovable("Anna"));
  EXPECT_EQ(s21vec[2], dummyMovable("Louie"));
  EXPECT_EQ(s21vec[3], dummyMovable("Noel"));

  s21vec.insert(s21vec.begin() + 3, dummyMovable("Emil"));
  EXPECT_EQ(s21vec[3], dummyMovable("Emil"));
  vec.insert(vec.begin() + 3, dummyMovable("Emil"));

  s21vec.erase(s21vec.end() - 1);
  s21vec.erase(s21vec.end() - 1);
  vec.erase(vec.end() - 1);
  vec.erase(vec.end() - 1);
  EXPECT_EQ(vec, s21vec);
}

TEST(VectorTest, scenario_ctors) {
  s21::vector<dummyMovable> vec{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };
  std::cout << "You should definetly listen to these names: " << std::endl;
  std::cout << vec << std::endl;
  s21::vector<dummyMovable> cpy(vec);
  EXPECT_EQ(vec, cpy);
  s21::vector<dummyMovable> mv(std::move(vec));
  EXPECT_EQ(mv, cpy);

  s21::vector<dummyMovable> cpy_ass;
  cpy_ass = cpy;
  s21::vector<dummyMovable> mv_ass;
  mv_ass = std::move(mv);
  EXPECT_EQ(mv_ass, cpy_ass);
}

TEST(VectorTest, scenario_utility) {
  s21::vector<dummyMovable> vec{
      dummyMovable("Aileen"), dummyMovable("Anna"),  dummyMovable("Louie"),
      dummyMovable("Noel"),   dummyMovable("Grace"),
  };

  EXPECT_FALSE(vec.empty());
  vec.clear();
  EXPECT_TRUE(vec.empty());

  EXPECT_NE(vec.data(), nullptr);

  s21::vector<dummyMovable> other{
      dummyMovable(), dummyMovable(),  dummyMovable(),
  };

  vec.swap(other);

  for (auto i: vec)
    EXPECT_EQ(i, dummyMovable());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
