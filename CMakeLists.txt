cmake_minimum_required(VERSION 3.21)
project(ChessGit)

set(CMAKE_CXX_STANDARD 20)

add_executable(ChessGit main.cpp attacks.cpp board.cpp move.cpp test.cpp utility.cpp)
