#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../Data.h"

// Простая проверка set и get
TEST_CASE("Data set/get works correctly") {
Data data(10);

data.set(5, 42);
REQUIRE(data.get(5) == 42);

data.set(9, 100);
REQUIRE(data.get(9) == 100);
}

// Проверка на границы
TEST_CASE("Data respects bounds") {
Data data(5);
data.set(10, 77);  // вне границ — ничего не должно произойти
REQUIRE(data.get(10) == 0);
}

// Проверка очистки по ID
TEST_CASE("Data clears marks by ID") {
Data data(10);
data.set(3, 7);
data.set(4, 7);
data.set(5, 2);

data.clearMarksBy(7);

REQUIRE(data.get(3) == 0);
REQUIRE(data.get(4) == 0);
REQUIRE(data.get(5) == 2);
}
