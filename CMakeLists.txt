cmake_minimum_required(VERSION 3.0.0)
project(puzzle-loop-solver VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(puzzle-loop-solver main.cpp puzzle.h puzzle_solver.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
