// list_reverse.h
#pragma once

struct Node {
    int value;
    Node* next;
};

Node* reverse_recursive(Node* head);
