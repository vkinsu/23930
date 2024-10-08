#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 1024

typedef struct List{
    char* str;
    struct List* next;
} List;

List* InitList(){
    List* elem = (List*)malloc(sizeof(List));
    elem ->next = NULL;
    elem->str = NULL;
    return elem;
}

List* Push(List* cur_elem, char* cur_str){
    cur_elem->str = (char*)malloc(sizeof(char) * (strlen(cur_str) + 1));
    if (!cur_elem -> str){
        printf("Failed to allocate memory.\n");
        return 0;
    } 
    strcpy(cur_elem->str, cur_str);
    List* new_elem = (List*)malloc(sizeof(List));
    if (!new_elem){
        printf("Failed to allocate memory.\n");
        return 0;
    }
    new_elem->next = NULL;
    new_elem->str = NULL;
    cur_elem->next = new_elem;
    return new_elem;
}

void PrintList(List *head){
    printf("\n");
    List *cur_elem = head;
    while(cur_elem->next != NULL){
        printf("%s", cur_elem->str);
        cur_elem = cur_elem->next;
    }
}

void FreeList(List *head){
    List *cur_elem = head;
    List* next;
    while(cur_elem->next != NULL){
        next = cur_elem->next;
        free(cur_elem->str);
        free(cur_elem);
        cur_elem = next;
    }
}

int main(){
    List* head = InitList();
    char cur_str[MAX_LENGTH];
    List* cur_elem = head;
    while(fgets(cur_str, MAX_LENGTH, stdin) != NULL){
        if (cur_str[0] == '.') {
            break;
        }
        cur_elem = Push(cur_elem, cur_str);
    }

    PrintList(head);
    FreeList(head);
    return 0;
}
