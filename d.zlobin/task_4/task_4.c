#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

Node* create_node(const char *input_str) {
    Node *new_node = (Node*) malloc(sizeof(Node));
    if (!new_node) {
        perror("Memory allocation error!\n");
        exit(EXIT_FAILURE);
    }
    
    size_t len = strlen(input_str);
    new_node->str = (char *) malloc((len + 1) * sizeof(char));
    if (!new_node->str) {
        perror("Memory allocation error!\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(new_node->str, input_str);
    new_node->next = NULL;
    
    return new_node;
}

void append_node(Node **head, const char *input_str) {
    Node *new_node = create_node(input_str);
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s", current->str);
        current = current->next;
    }
}

void free_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
}

int main() {
    Node *head = NULL;
    char input[BUFFER_SIZE];

    printf("Write here strings (put '.' to end the input data):\n");

    while (1) {
        fgets(input, sizeof(input), stdin);
        
        if (input[0] == '.') {
            break;
        }

        append_node(&head, input);
    }

    printf("\nInput strings:\n");
    print_list(head);

    free_list(head);

    return 0;
}
