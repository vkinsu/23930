#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char *str;
    struct Node *next;
} Node;

void insert(Node **head, char *str)
{
    Node *newNode = malloc(sizeof(Node));
    newNode->str = malloc(strlen(str) + 1);
    strcpy(newNode->str, str);
    newNode->next = *head;
    *head = newNode;
}

void printList(Node *head)
{
    Node *current = head;
    while (current != NULL)
    {
        printf("%s\n", current->str);
        current = current->next;
    }
}

void freeList(Node *head)
{
    Node *current = head;
    Node *next;
    while (current != NULL)
    {
        next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
}

int main()
{
    Node *head = NULL;
    char buffer[1024];
    while (1)
    {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, ".") == 0)
            break;
        insert(&head, buffer);
    }
    printList(head);
    freeList(head);
    return 0;
}
