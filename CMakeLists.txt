cmake_minimum_required(VERSION 3.10)
project(2048_CPP)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -m64 -mbmi2 -pthread -O3")
add_executable(2048_CPP main.cpp)

# g++ -std=c++17 -g -m64 -mbmi2 -pthread -O3 -o 2048_CPP ../main.cpp