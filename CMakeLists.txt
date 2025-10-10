cmake_minimum_required(VERSION 3.15)

project(TapeSort VERSION 0.5 LANGUAGES CXX)

add_executable(
    tape-sort
    tape-sort/main.cpp
    tape-sort/src/placeholder.cpp
)

target_include_directories(
    tape-sort PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tape-sort/include
)
