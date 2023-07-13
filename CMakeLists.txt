cmake_minimum_required(VERSION 3.0)
project(containers)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-g -fdiagnostics-color=always -fsanitize=address")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/Wall")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

if (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif (WIN32)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCE test_main.cc)

list(TRANSFORM TEST_SOURCE PREPEND tests/)

add_executable(containers ${TEST_SOURCE})
target_include_directories(containers PRIVATE "include")
target_link_libraries(containers GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(containers)
