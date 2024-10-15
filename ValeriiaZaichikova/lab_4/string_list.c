#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

void addNode (Node *curr, char *data) {
    char *tmp = (char*) malloc ((strlen(data) + 1) * sizeof(char));
    strcpy(tmp, data);

    curr->str = tmp;
    curr->next = (Node*) malloc (sizeof(Node));
}

void processInput (Node* head) {
    char inputBuffer [MAX_SIZE] = {0}; 

    Node* curr = head;

    while (
        fgets(inputBuffer, MAX_SIZE, stdin) != NULL) {

        if (inputBuffer[0] == '.') {
            return;
        }
        
        char *lineEnd = strchr(inputBuffer, '\n');
        
        if (lineEnd == NULL) {
            size_t newBufSize = 0;
            size_t newBufCap = MAX_SIZE;
            char *newBuf = malloc(MAX_SIZE);

            memcpy(newBuf, inputBuffer, MAX_SIZE);
            newBufSize += MAX_SIZE - 1;

            while (
                fgets(inputBuffer, MAX_SIZE, stdin) != NULL) {

                newBufCap += MAX_SIZE;
                newBuf = realloc(newBuf, newBufCap);

                memcpy(newBuf + newBufSize, inputBuffer, MAX_SIZE);
                newBufSize += MAX_SIZE - 1;

                lineEnd = strchr(newBuf, '\n');
                if (lineEnd) {
                    *lineEnd = '\0';
                    addNode(curr, newBuf);
                    curr = curr->next;
                    free(newBuf);
                    break;
                }
            }
        } else {
            *lineEnd = '\0';
            addNode(curr, inputBuffer);
            curr = curr->next;
        }
    }
}

void printList (Node* head) {
    Node *ptr = head;
    while (ptr != NULL) {
        if (ptr->str) {
            printf("%s\n", ptr->str);
        }
        ptr = ptr->next;
    }
}

int main () {
    Node* head = (Node*) malloc (sizeof(Node));
    head->str = NULL;
    head->next = (Node*) malloc (sizeof(Node));

    processInput (head);

    printList (head);

    return 0;
}