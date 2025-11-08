cmake_minimum_required(VERSION 3.10)
project(Ascii-orbit)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(Ascii-orbit
        src/main.cpp
)

# If you're on Windows and using Windows-specific features
if(WIN32)
    target_compile_definitions(Ascii-orbit PRIVATE WINDOWS_BUILD)
endif()

# Optional: Add compiler warnings for better code quality
if(MSVC)
    target_compile_options(Ascii-orbit PRIVATE /W4)
else()
    target_compile_options(Ascii-orbit PRIVATE -Wall -Wextra -Wpedantic)
endif()