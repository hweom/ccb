cmake_minimum_required(VERSION 2.8)

project(ccb)

#set(CMAKE_C_COMPILER "clang")
#set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -gdwarf-2")

#To build in Release mode, run "cmake -DCMAKE_BUILD_TYPE=Release"
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else()
    set(CMAKE_BUILD_TYPE, "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Compile using c++ 11 standart features.
add_definitions(
    -std=c++11
)

# List of directories with header files.
include_directories(
    src
)

# Library
file(GLOB LIBRARY_LIST
    src/${PROJECT_NAME}/*.?pp
    src/${PROJECT_NAME}/filesystem/*.?pp
    src/${PROJECT_NAME}/filesystem/unix/*.?pp
    src/${PROJECT_NAME}/log/*.?pp
)
add_library(${PROJECT_NAME} ${LIBRARY_LIST})

# cxxtest
find_package(CxxTest)
if(CXXTEST_FOUND)
    set(CXXTEST_USE_PYTHON TRUE)
    set(CXXTEST_TESTGEN_ARGS --error-printer -f)
    include_directories(${CXXTEST_INCLUDE_DIR})
    file(GLOB_RECURSE UNITTEST_LIST src/${PROJECT_NAME}_tests/*.?pp)
    enable_testing()
    CXXTEST_ADD_TEST(${PROJECT_NAME}_tests Tests.cpp ${UNITTEST_LIST})
    target_link_libraries(${PROJECT_NAME}_tests)
endif()
