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

if ((CMAKE_CXX_COMPILER_ID STREQUAL GNU) OR (CMAKE_CXX_COMPILER_ID STREQUAL AppleClang))
  add_compile_options(-Wall -Werror -Wextra -g -fdiagnostics-color=always -fsanitize=address)
  add_link_options(-fsanitize=address)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  add_compile_options(/fsanitize=address)
endif()


add_executable(
  test_vector
  tests/test_vector.cc
)
target_link_libraries(
    test_vector
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_vector)
