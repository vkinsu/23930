#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 256

struct Node {
    char *value;
    struct Node *next;
    struct Node *prev;
};

void addNode(struct Node **head, char *str) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate memory for node\n");
        exit(1);
    }

    newNode->value = (char *)malloc(strlen(str) + 1);
    if (!newNode->value) {
        fprintf(stderr, "Failed to allocate memory for string\n");
        exit(1);
    }

    strcpy(newNode->value, str);
    newNode->next = *head;
    newNode->prev = newNode;
    if ((*head) != NULL) {
        newNode->prev = (*head)->prev;
        (*head)->prev = newNode;
    }
    *head = newNode;
}

void printList(struct Node *head) {
    struct Node *current = head->prev;
    while (1) {
        printf("%s", current->value);
        current = current->prev;
        if (current == head) {
            if (current->next != NULL) {
                printf("%s", current->value);
            }
            break;
        }
    }
}

void freeList(struct Node *head) {
    struct Node *tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp->value);
        free(tmp);
    }
}

int main() {
    struct Node *head = NULL;
    char buffer[MAX_STR_LEN];

    printf("Enter the lines (enter '.' to complete entry):\n");

    while (1) {
        fgets(buffer, MAX_STR_LEN, stdin);

        if (buffer[0] == '.') { // Better to check for '\0' or '\n' after '.'
            break;
        }

        addNode(&head, buffer);
    }

    if (head == NULL) {
        printf("\nList is empty\n");
        exit(0);
    }

    printf("\nEntered list:\n");
    printList(head);
    freeList(head);
    head = NULL;

    exit(0);
}