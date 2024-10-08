#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LEN 256


typedef struct String{
    char string[STRING_LEN];
    struct String *next;
}String;

typedef struct Strings{
    String *first;
    String *last;
}Strings;


Strings* init_strings(){
    Strings *strings = malloc(sizeof(Strings));
    return strings;
}

void free_strings(Strings *strings){
    String *tmp_next;
    while (strings->first != NULL){
        printf("%s", strings->first->string);
        tmp_next = strings->first->next;
        free(strings->first);
        strings->first = tmp_next;
    }
    
    free(strings);
}


void add_to_strings(Strings *strings, char* current_str){
    String *next_str = malloc(sizeof(String));
    next_str->next = NULL;
    strcpy(next_str->string, current_str);
    if (strings->first == NULL){
        strings->first = next_str;
        strings->last = next_str;
        return;
    }
    strings->last->next = next_str;
    strings->last = next_str;
}

int main(){
    Strings *strings = init_strings();
    char input_str[STRING_LEN];
    while (fgets(input_str, sizeof(input_str), stdin) != NULL){
        if (input_str[0] == '.') break;

        add_to_strings(strings, input_str);
    }
    free_strings(strings);
    return 0;
}