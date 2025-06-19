// list_reverse.cpp
#include "list_reverse.h"

Node* reverse_recursive(Node* head) {
    if (!head || !head->next)
        return head;

    Node* new_head = reverse_recursive(head->next);
    head->next->next = head;
    head->next = nullptr;
    return new_head;
}
