#include <stdio.h>
#include <stdlib.h>

typedef struct List_s { 
    char* str; 
    struct List* next; 
} List;

int main() {
    List* head, *tail, *p;
    char* input;

    head = (List*)malloc(sizeof(List));
    head->next = NULL;
    p = head;
    tail = head;

    while (input == NULL || input[0] != '.') {
        input = (char*)malloc(100 + 1);
        fgets(input, 101, stdin);
        p->str = input;
        p->next = (List*)malloc(sizeof(List));
        p = p->next;
        p->next = NULL;
    }

    tail = p;
    p = head;
    printf("\n");

    while (p != tail) {
        printf("%s", p->str);
        free(p->str);

        if (p->next == NULL) {
            break;
        }
        p = p->next;
    }

    return 0;
}