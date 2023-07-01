cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} "-Wall -Werror -Wextra -g -fdiagnostics-color=always")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/Wall")
endif()

# set(CMAKE_TOOLCHAIN_FILE "C:/Users/Lord/source/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(containers)

# find_package(GTest REQUIRED)

set(TEST_SOURCE main.cc)

add_library(containers STATIC ${SOURCE})

add_executable(containers ${TEST_SOURCE})

target_link_libraries(containers s21_matrix)