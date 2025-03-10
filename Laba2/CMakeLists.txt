cmake_minimum_required(VERSION 3.10)
project(Laba2)

set(CMAKE_CXX_STANDARD 11)

# Указываем, что в src/ лежат заголовочные файлы
include_directories(${CMAKE_SOURCE_DIR}/src)

# Основная программа (указывать ВСЕ .cpp файлы!)
add_executable(main main.cpp src/functions.cpp)

# Тесты (отдельный исполняемый файл)
add_executable(test_run tests/test.cpp src/functions.cpp)

# Указываем, что тесты используют заголовки из src/ и библиотеки из libs/
target_include_directories(test_run PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/libs)
