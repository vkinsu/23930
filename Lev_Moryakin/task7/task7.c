#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(int argc,char **argv){
    char str[1];
    char *outstr;
    int file = open(argv[1],O_RDONLY);
    char *fileptr;
    fileptr = (char*)mmap(0, 1000, PROT_READ, MAP_SHARED,file, 0);
    int bytesread = 0;
    int *newlines = (int*)malloc(sizeof(int) * 1);
    newlines[0] = 0;
    int newlinessize = 1;
    int iline = 1;
    int showline;
    int startpos,endpos;
    while(fileptr[bytesread] != '\0'){
        //printf("%c ",str[0]);
        if(fileptr[bytesread] == '\n'){
            if(newlinessize <= iline + 1){
                newlinessize *= 2;
                newlines = (int*)realloc(newlines, newlinessize * sizeof(int));
            }
            newlines[iline++] = bytesread+1; //позиция переноса строки в файле
        }
        bytesread++;
    }
    //printf("\n");
    //for(int i = 0; i < iline; i++) printf("%d\n",newlines[i]);
    while(1){
        scanf("%d",&showline);
        if(showline == 0)return 0;
        printf("%d-%d\n",newlines[showline - 1],newlines[showline]);
        if(!showline)break;
        startpos = newlines[showline - 1];
        endpos = newlines[showline];
        for(int i = startpos; i < endpos;i++){
            printf("%c",fileptr[i]);
        }
    }
    close(file);
    return 0;
}