#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STRING_LENGTH 1000

typedef struct Node_s{
    char* str;
    struct Node_s* next;
} Node;

Node* InitList() {
    Node *start_node = malloc(sizeof(Node));
    start_node->next = NULL;
    start_node->str = NULL;
    return start_node;
}

Node* Push(char *current_string, Node *current_node) {
    current_node->str = malloc(sizeof(char) * (strlen(current_string) + 1));
    strcpy(current_node->str, current_string);
    Node *new_node = malloc(sizeof(Node));
    new_node->next = NULL;
    new_node->str = NULL;
    current_node->next = new_node;
    return new_node;
}

void PrintList(Node *head) {
    Node *current_node = head;
    while(current_node->next != NULL) {
        printf("%s", current_node->str);
        current_node = current_node->next;
    }
}

void CleanList(Node *head) {
    Node *current_node = head;
    while(current_node->next != NULL) {
        free(current_node->str);
        current_node = current_node->next;
    }
}

int main() {
    Node *head = InitList();
    Node *current_node = head;
    char current_string[STRING_LENGTH] = "";
    while(fgets(current_string, STRING_LENGTH, stdin) != NULL) {
        if (current_string[0] == '.') {
            break;
        }
        current_node = Push(current_string, current_node);
    }

    PrintList(head);
    CleanList(head);
    return 0;
}