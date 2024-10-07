#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char* data;
    struct Node* next;
};

void insert(struct Node** head, const char* str) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }

    newNode->data = (char*)malloc(strlen(str) + 1);
    if (!newNode->data) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }

    strcpy(newNode->data, str);
    newNode->next = *head;
    *head = newNode;
}

void printList(const struct Node* head) {
    const struct Node* current = head;
    while (current) {
        printf("%s", current->data);
        current = current->next;
    }
}

void freeList(struct Node* head) {
    struct Node* current = head;
    while (current) {
        struct Node* nextNode = current->next;
        free(current->data);
        free(current);
        current = nextNode;
    }
}

int main() {
    struct Node* head = NULL;
    char buffer[256]; 

    printf("Введите строки (для завершения введите строку, начинающуюся с '.'): \n");
    
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Ошибка чтения строки\n");
            break;
        }

        if (buffer[0] == '.') {
            break;
        }

        insert(&head, buffer);
    }

    printf("\nВведенные строки:\n");
    printList(head); 

    freeList(head);

    return 0;
}
