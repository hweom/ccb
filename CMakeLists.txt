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

# Linux-specific definitions
if(UNIX)
    add_definitions(-D_NOEXCEPT=noexcept)
endif()

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
    src/${PROJECT_NAME}/crypt/*.?pp
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
    file(GLOB UNITTEST_LIST
        src/${PROJECT_NAME}_tests/binary/*.?pp
        src/${PROJECT_NAME}_tests/charset/*.?pp
        src/${PROJECT_NAME}_tests/config/*.?pp
        src/${PROJECT_NAME}_tests/csv/*.?pp
        src/${PROJECT_NAME}_tests/filesystem/*.?pp
        src/${PROJECT_NAME}_tests/image/*.?pp
        src/${PROJECT_NAME}_tests/stream/*.?pp
    )

    set(TEST_LIBS ${PROJECT_NAME})

    find_package(OpenSSL)
    if (OPENSSL_FOUND)
        file(GLOB UNITTEST_CRYPT_LIST
            src/${PROJECT_NAME}_tests/crypt/*.?pp
        )

        set(TEST_LIBS ${TEST_LIBS} ${OPENSSL_LIBRARIES})
    endif()

    set(CXXTEST_USE_PYTHON TRUE)
    set(CXXTEST_TESTGEN_ARGS --error-printer -f)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()
    CXXTEST_ADD_TEST(${PROJECT_NAME}_tests Tests.cpp ${UNITTEST_LIST} ${UNITTEST_CRYPT_LIST})
    target_link_libraries(${PROJECT_NAME}_tests ${TEST_LIBS})
endif()
