cmake_minimum_required(VERSION 3.10)
project(MarkerThreads)

set(CMAKE_CXX_STANDARD 03)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MarkerThreads
        main.cpp
        ThreadController.cpp
)

if (WIN32)
    target_link_libraries(MarkerThreads
            kernel32
            user32
    )
endif()

# === Тесты ===
enable_testing()
add_executable(DataTests
        tests/test_data.cpp
)

# Для WinAPI тестов может понадобиться линковка (если ты расширишь логику)
if (WIN32)
    target_link_libraries(DataTests
            kernel32
    )
endif()

add_test(NAME DataTests COMMAND DataTests)
