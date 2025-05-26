cmake_minimum_required(VERSION 3.10)
project(Laba4_98 CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Общие исходники
set(SOURCES_COMMON
        RingBuffer.cpp
)

set(HEADERS
        RingBuffer.hpp
        MyProcess.h
        MyEvent.h
)

# Receiver
add_executable(Receiver
        Receiver.cpp
        ${SOURCES_COMMON}
        ${HEADERS}
)
target_link_libraries(Receiver PRIVATE ws2_32)

# Sender
add_executable(Sender
        Sender.cpp
        ${SOURCES_COMMON}
        ${HEADERS}
)
target_link_libraries(Sender PRIVATE ws2_32)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# UnitTests
add_executable(UnitTests
        tests/test_ringbuffer.cpp
        RingBuffer.cpp
)
target_include_directories(UnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UnitTests gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(UnitTests)
