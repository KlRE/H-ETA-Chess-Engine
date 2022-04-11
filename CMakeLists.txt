cmake_minimum_required(VERSION 3.16)
project(H:Eta)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(src)
link_directories(src)
add_subdirectory(src)

add_executable(HEta main.cpp)

target_link_libraries(HEta src)

add_subdirectory(Google_Test)