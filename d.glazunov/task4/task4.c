#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Определяем структуру узла списка
struct Node {
    char *data;          // Указатель на строку
    struct Node *next;   // Указатель на следующий узел
};

// Функция для создания нового узла списка
struct Node* create_node(char *str) {
    struct Node *new_node = (struct Node*) malloc(sizeof(struct Node));
    if (!new_node) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    new_node->data = str;
    new_node->next = NULL;
    return new_node;
}

// Функция для добавления узла в конец списка
void append(struct Node **head, char *str) {
    struct Node *new_node = create_node(str);
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        struct Node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

// Функция для вывода всех строк из списка
void print_list(struct Node *head) {
    struct Node *temp = head;
    while (temp != NULL) {
        printf("%s", temp->data);
        temp = temp->next;
    }
}

// Освобождение памяти, выделенной под список
void free_list(struct Node *head) {
    struct Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->data);  // Освобождаем память, выделенную под строку
        free(temp);        // Освобождаем память, выделенную под узел
    }
}

int main() {
    struct Node *head = NULL;
    char buffer[1024];  // Массив для хранения временной строки

    printf("Enter lines (to complete, enter a line starting with a dot '.'): \n");

    // Ввод строк с клавиатуры
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Проверка на завершение ввода
        if (buffer[0] == '.') {
            break;
        }

        // Выделяем память для новой строки
        char *str = (char*) malloc(strlen(buffer) + 1);
        if (!str) {
            perror("Failed to allocate memory for string");
            exit(EXIT_FAILURE);
        }

        // Копируем строку в динамически выделенную память
        strcpy(str, buffer);

        // Добавляем строку в список
        append(&head, str);
    }

    // Вывод всех строк из списка
    printf("\nEntered strings:\n");
    print_list(head);

    // Освобождение памяти
    free_list(head);

    return 0;
}
