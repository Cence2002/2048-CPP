
# Reinfocement Learning Framework for 2048

## Description

[The Puzzle Game of 2048](https://play2048.co/) offers a deeply engaging and strategically complex challenge despite its minimalist design and simplistic rule set. Thanks to this addictive nature, it has reached over 23 million players since its release in 2014.

2048 has also experienced strong interest among researchers, both as a challenge to develop the best-performing algorithm and also as an excellent platform to benchmark Reinforcement Learning techniques.

The core objectives of this project were to examine the mathematical properties of 2048, to implement a highly optimised simulation framework, and to develop a well-performing algorithm by combining various Reinforcement Learning approaches with Computational Mathematics and experimentally perfected novel contributions.

## Installation and Setup

1. Clone the repository to your local machine with:

```git clone https://github.com/Cence2002/2048_CPP.git```

2. Navigate to the root directory of the repository:

```cd 2048_CPP```

3. Create new build directory and navigate to it:

```mkdir build && cd build```

4. Run CMake to generate the build files:

```cmake ..```

5. Build the project:

```make```

6. Run the built executable to run the program:

```./2048```

7. Alternatively, run the other executable to perform validity tests:

```./tests```

8. To clean the build files, run:

```make clean```


## Customisation

The testing and training configurations are intuitively customisable at the top of `main.cpp` where these setting are abstracted into `constexpr` variables to avoid sacrificing performance.

## Results

Combining all value approximation enhancement strategies wit two novel additions - PNP and BFCS -- our method can achieve:
- Average score over 600,000, over 95% of the current state-of-the-art (reaching an average of 625,000)
- Using only 1-2 milliseconds per move, which is less, than 1-2% of the thinking time of the current state-of-the-art (using 0.4 seconds per move)

These results make our solution one of the most efficient algorithm for 2048, and arguably the most sophisticated decision-making strategy to date.

## Motivation

This project was completed as a prerequisite for Part II of Computer Science Tripos (2024) at the University of Cambridge.

## License

The project is protected by the standard MIT License, which is included in the repository and can be found below:

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)