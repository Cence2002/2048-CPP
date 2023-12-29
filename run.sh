#!/bin/bash

# Clear log
> output.log

# Create build directory
mkdir -p cmake_build
cd cmake_build

# Build
cmake ..
make

# Run
nohup ./2048_CPP &
