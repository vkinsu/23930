#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct List{
    char* string;
    struct List* next;
} List;

void Append(List ** head, char* string) {
    string[strlen(string)-1] = '\0';
    List* new_node = (List*)malloc(sizeof(List));
    if (!new_node) {
        printf("Failed to allocate memory.\n");
        return;
    }
    new_node->next = NULL;
    new_node->string = (char*)malloc(strlen(string) + 1);
    if (!new_node->string) {
        printf("Failed to allocate memory for string.\n");
        return;
    }
    strcpy(new_node->string, string);
    if (*head == NULL) {
        *head = new_node;
    }
    else {
        List * last = *head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_node;
    }
}

void PrintStrings(List * head) {
    List * cur = head;
    while (cur != NULL) {
        printf("%s\n", cur->string);
        cur = cur->next;
    }
}

void FreeList(List * head) {
    List * cur = head;
    List * next;
    while (cur != NULL) {
        next = cur->next;
        free(cur->string);
        free(cur);
        cur = next;
    }
}

int main() {
    char array[1024];
    List * strings = NULL;
    while (1) {
        fgets(array, sizeof(array), stdin);
        if (array[0] == '.') {
            Append(&strings, array);
            break;
        }
        Append(&strings, array);
    }
    printf("\n");
    PrintStrings(strings);
    FreeList(strings);
    return 0;
}