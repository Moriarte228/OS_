cmake_minimum_required(VERSION 3.20)
project(Laba1)

set(CMAKE_CXX_STANDARD 98)

add_executable(creator src/creator.cpp)
add_executable(reporter src/reporter.cpp)
add_executable(main src/main.cpp)

target_include_directories(creator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(reporter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src)
