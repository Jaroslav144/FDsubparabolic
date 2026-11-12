cmake_minimum_required(VERSION 3.16)
project(FDsubparabolic CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FDsubparabolic
    src/main.cpp
    src/Grid.cpp
    src/DiffusivityModel.cpp
    src/Parameters.cpp
    src/Output.cpp
    src/Solver.cpp
    src/IO.cpp
    src/TridiagonalSolver.cpp
)

target_include_directories(FDsubparabolic PRIVATE include)
