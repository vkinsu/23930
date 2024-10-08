#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char **lines = (char**)malloc(sizeof(char*));
    int count = 0;
    char buffer[256];
    printf("Enter lines:\n");
    while(1){
        fgets(buffer,256,stdin);
        count++;
        if (buffer[0] == '.') break;
        lines = (char**)realloc(lines,(count+1)*sizeof(char*));
        lines[count-1] = (char*)malloc(strlen(buffer)+1);
        strcpy(lines[count-1], buffer);
    }
    printf("lines:\n");
    for (int i = 0; i < count-1; ++i) printf("%s", lines[i]);
    for (int i = 0; i < count; ++i) free(lines[i]);
    free(lines);
    return 0;
}
