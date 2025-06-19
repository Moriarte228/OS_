cmake_minimum_required(VERSION 3.20)
project(IndustrialCode)

set(CMAKE_CXX_STANDARD 20)
enable_testing()

# === Подключение GoogleTest ===
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# === Основная библиотека проекта ===
add_library(core
        factorials.cpp
        unique_elements.cpp
        list_reverse.cpp
)

# === Тесты ===
add_executable(tests test_main.cpp)
target_link_libraries(tests core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
