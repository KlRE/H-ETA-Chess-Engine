cmake_minimum_required(VERSION 3.21)
project(ChessGit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(src)
link_directories(src)
add_subdirectory(src)

add_executable(ChessGit main.cpp)

target_link_libraries(ChessGit src)

add_subdirectory(Google_Test)