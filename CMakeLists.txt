cmake_minimum_required(VERSION 3.14)

# Set the main project
project(2048_CPP)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set the compiler flags, most crucially the -O3 flag for optimization and -mbmi2 for BMI2 (e.g. PEXT)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -m64 -mbmi2 -pthread -O3")
add_executable(2048_CPP main.cpp)

# Automatically fetch and build googletest
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/d83fee138a9ae6cb7c03688a2d08d4043a39815d.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Setup testing with gtest
enable_testing()
add_executable(tests tests.cpp)
target_link_libraries(tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)

# g++ -std=c++17 -g -m64 -mbmi2 -pthread -O3 -o 2048_CPP ../main.cpp
