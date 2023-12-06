cmake_minimum_required(VERSION 3.0...3.5)
project(containers)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)

include(EnableGoogleTest)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# valgrind  --tool=memcheck --track-fds=yes --trace-children=yes --track-origins=yes --leak-check=full --show-leak-kinds=all -s --log-file=leak_report.txt ./test_vector
  add_compile_options(-Wall -Werror -Wextra -Wimplicit-fallthrough -Wpedantic)
  # add_link_options(-fsanitize=address)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
  add_compile_options(-Wall -Werror -Wextra -Wimplicit-fallthrough -Wpedantic -O3)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/W4 /EHsc /fsanitize=address)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  add_compile_options(-Wall -Werror -Wextra -Wimplicit-fallthrough -Wpedantic -O3)
  # add_link_options(-fsanitize=address)
endif()


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
