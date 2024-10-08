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
        inpstr = (char*)malloc(100 + 1);
        fgets(inpstr, 101, stdin);
        p->str = inpstr;
        p->next = (list*)malloc(sizeof(list));
        p = p->next;
        p->next = NULL;
    } while (inpstr == NULL || inpstr[0] != '.');
    tail = p;

    p = head;
    while (p != tail) {
        printf("%s\n", p->str);
        if(p->next == NULL)break;
        p = p->next;
    } 
    return 0;
}