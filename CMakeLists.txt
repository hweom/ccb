cmake_minimum_required(VERSION 2.8)

project(ccb)

#To build in Release mode, run "cmake -DCMAKE_BUILD_TYPE=Release"
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Compile using c++ 11 standart features.
add_definitions(
    -std=c++11
    -pthread
)

# List of directories with header files.
include_directories(
    src
)

# Library
file(GLOB LIBRARY_LIST
    src/${PROJECT_NAME}/*.?pp
    src/${PROJECT_NAME}/binary/*.?pp
    src/${PROJECT_NAME}/charset/*.?pp
    src/${PROJECT_NAME}/config/*.?pp
    src/${PROJECT_NAME}/csv/*.?pp
    src/${PROJECT_NAME}/filesystem/*.?pp
    src/${PROJECT_NAME}/filesystem/unix/*.?pp
    src/${PROJECT_NAME}/image/*.?pp
    src/${PROJECT_NAME}/log/*.?pp
    src/${PROJECT_NAME}/meta/*.?pp
    src/${PROJECT_NAME}/mock/*.?pp
    src/${PROJECT_NAME}/stream/*.?pp
    src/${PROJECT_NAME}/tree/*.?pp
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
    target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})
endif()
