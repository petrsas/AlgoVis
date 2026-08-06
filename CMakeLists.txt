cmake_minimum_required(VERSION 3.22)

#for my nvim setup
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(AlgoVis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.1.0
)
FetchContent_MakeAvailable(SFML)

add_executable(AlgoVis src/algo_vis/main.cpp)
target_link_libraries(AlgoVis PRIVATE SFML::Graphics)
