#define CATCH_CONFIG_MAIN
#include "../src/functions.h"
#include "../libs/catch.hpp"

TEST_CASE("Проверка вычисления min/max", "[compute_min_max]") {
    SharedData data;
    data.arr = std::vector<int>{3, 1, 4, 1, 5, 9, 2};

    HANDLE thread = CreateThread(NULL, 0, compute_min_max, &data, 0, NULL);
    REQUIRE(thread != NULL);

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    REQUIRE(data.min_val == 1);
    REQUIRE(data.max_val == 9);
}

TEST_CASE("Проверка вычисления среднего", "[compute_average]") {
    SharedData data;
    data.arr = std::vector<int>{3, 1, 4, 1, 5, 9, 2};

    HANDLE thread = CreateThread(NULL, 0, compute_average, &data, 0, NULL);
    REQUIRE(thread != NULL);

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    REQUIRE(data.average == Approx(3.57).epsilon(0.01));
}

TEST_CASE("Проверка обновления массива", "[update_array]") {
    SharedData data;
    data.arr = std::vector<int>{3, 1, 4, 1, 5, 9, 2};
    data.min_val = 1;
    data.max_val = 9;
    data.average = 3.57;

    update_array(data);

    std::vector<int> ans_arr {3, 3, 4, 3, 5, 3, 2};
    REQUIRE(data.arr == ans_arr);
}



