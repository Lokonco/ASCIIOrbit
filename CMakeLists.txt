cmake_minimum_required(VERSION 3.29)
project(AsciiOrbit)
set(CMAKE_CXX_STANDARD 20)

add_executable(asciiorbit main.cpp)

install(TARGETS asciiorbit DESTINATION bin)
