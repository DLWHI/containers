cmake_minimum_required(VERSION 3.0)
project(containers)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

if (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (WIN32)
FetchContent_MakeAvailable(googletest)

enable_testing()

if ((CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    OR (CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
    OR (CMAKE_CXX_COMPILER_ID STREQUAL Clang))
  add_compile_options(-Wall -Werror -Wextra -g)
  # add_custom_target(leak_check)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  # add_compile_options(/Wall)
endif()

include(GoogleTest)

# add_executable(
#   test_pointer_iterator
#   tests/test_pointer_iterator.cc
# )
# target_link_libraries(
#   test_pointer_iterator
#   GTest::gtest_main
# )
# gtest_discover_tests(test_pointer_iterator)

add_executable(
  test_vector
  tests/test_vector.cc
)
target_link_libraries(
    test_vector
  GTest::gtest_main
)
gtest_discover_tests(test_vector)

add_executable(
  test_array
  tests/test_array.cc
)
target_link_libraries(
  test_array
  GTest::gtest_main
)
gtest_discover_tests(test_array)

add_executable(
  test_list
  tests/test_list.cc
)
target_link_libraries(
  test_list
  GTest::gtest_main
)
gtest_discover_tests(test_list)
