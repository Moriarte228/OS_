// test_main.cpp
#include <gtest/gtest.h>
#include "factorials.h"
#include "unique_elements.h"
#include "list_reverse.h"

TEST(FactorialTest, NormalCase) {
auto result = generate_factorials(5);
std::vector<unsigned long long> expected = {1, 2, 6, 24, 120};
EXPECT_EQ(result, expected);
}

TEST(FactorialTest, ZeroInputThrows) {
EXPECT_THROW(generate_factorials(0), std::invalid_argument);
}

TEST(RemoveDuplicatesTest, WorksCorrectly) {
std::vector<int> input = {1, 2, 2, 3, 1};
std::vector<int> expected = {1, 2, 3};
EXPECT_EQ(remove_duplicates(input), expected);
}

TEST(LinkedListTest, ReverseWorks) {
Node a{1}, b{2}, c{3};
a.next = &b;
b.next = &c;
c.next = nullptr;

Node* new_head = reverse_recursive(&a);

ASSERT_NE(new_head, nullptr);
EXPECT_EQ(new_head->value, 3);
EXPECT_EQ(new_head->next->value, 2);
EXPECT_EQ(new_head->next->next->value, 1);
EXPECT_EQ(new_head->next->next->next, nullptr);
}
