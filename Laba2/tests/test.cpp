#define CATCH_CONFIG_MAIN
#include "../src/functions.h"
#include "../libs/catch.hpp"
#include <algorithm>  // для std::equal

TEST_CASE("Проверка вычисления min/max", "[compute_min_max]") {
    SharedData data;
    int temp[] = {3, 1, 4, 1, 5, 9, 2};
    data.arr.assign(temp, temp + 7);

    HANDLE thread = CreateThread(NULL, 0, compute_min_max, &data, 0, NULL);
    REQUIRE(thread != NULL);

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    REQUIRE(data.min_val == 1);
    REQUIRE(data.max_val == 9);
}

TEST_CASE("Проверка вычисления среднего", "[compute_average]") {
    SharedData data;
    int temp[] = {3, 1, 4, 1, 5, 9, 2};
    data.arr.assign(temp, temp + 7);

    HANDLE thread = CreateThread(NULL, 0, compute_average, &data, 0, NULL);
    REQUIRE(thread != NULL);

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    REQUIRE(data.average == Approx(3.57).epsilon(0.01));
}

TEST_CASE("Проверка обновления массива", "[update_array]") {
    SharedData data;
    int temp1[] = {3, 1, 4, 1, 5, 9, 2};
    data.arr.assign(temp1, temp1 + 7);
    data.min_val = 1;
    data.max_val = 9;
    data.average = 3.57;

    update_array(data);

    int temp2[] = {3, 3, 4, 3, 5, 3, 2};
    std::vector<int> ans_arr;
    ans_arr.assign(temp2, temp2 + 7);

    REQUIRE(std::equal(data.arr.begin(), data.arr.end(), ans_arr.begin()));
}
