#include <stdio.h>
#include <stdlib.h>
struct list { char* str; struct list* next; };
typedef struct list list;
list* head, *tail, *p;

int main() {
    head = (list*)malloc(sizeof(list));
    head->next = NULL;
    p = head;
    tail = head;
    char* inpstr;
    do{
        inpstr = (char*)malloc(1000 + 1);
        fgets(inpstr, 1000, stdin);
        for(int i = 0; inpstr[i] != '\0'; i++){
            if(inpstr[i] == '\n'){
                inpstr[i] ='\0';
                break;
            }
        }
        p->str = inpstr;
        p->next = (list*)malloc(sizeof(list));
        p = p->next;
        p->next = NULL;
    } while (inpstr == NULL || inpstr[0] != '.');
    tail = p;
    printf("---------------\n");
    p = head;
    while (p->str[0] != '.') {
        if(p->str[0] != '\0')printf("%s\n", p->str);
        if(p->next == NULL)break;
        p = p->next;
    } 
    return 0;
}