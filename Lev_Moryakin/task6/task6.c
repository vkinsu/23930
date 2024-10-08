#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

int ahtung = 0;
void alarm_handle(int signal){
    ahtung = 1;
}

int main(int argc,char **argv){
    signal( SIGALRM, alarm_handle );
    char showlinestring[5];
    char *strptr;
    int istr;
    char str[1];
    char strout[100];
    char *outstr;
    int file = open(argv[1],O_RDONLY);
    int bytesread = 0;
    int *newlines = (int*)malloc(sizeof(int) * 1);
    newlines[0] = 0;
    int newlinessize = 1;
    int iline = 1;
    int showline;
    while(read(file,str,1)){
        //printf("%c ",str[0]);
        if(str[0] == '\n'){
            if(newlinessize <= iline + 1){
                newlinessize *= 2;
                newlines = (int*)realloc(newlines, newlinessize * sizeof(int));
            }
            newlines[iline++] = bytesread + 1; //позиция переноса строки в файле
        }
        bytesread++;
    }
    //printf("\n");
    //for(int i = 0; i < iline; i++) printf("%d\n",newlines[i]);
    while(1){
        alarm(5);
        while(1){
            istr = read(0,showlinestring,5);
            if(istr > 0){ showlinestring[istr - 1] = '\0'; break; }
            if(ahtung){
                printf("\nYOUR TIME IS OVER\n\n");
                lseek(file,0,SEEK_SET);
                while(read(file,strout,100))printf("%s",strout);
                return 0;
            }
        }
        showline = atoi(showlinestring);
        if(showline == 0)return 0;
        //scanf("%d",&showline);
        printf("%d-%d\n",newlines[showline - 1],newlines[showline]);
        if(!showline)break;
        lseek(file,newlines[showline - 1],SEEK_SET);
        outstr = (char*)malloc(newlines[showline] - newlines[showline - 1] + 1);
        bytesread = read(file,outstr,newlines[showline] - newlines[showline - 1]);
        outstr[bytesread] = '\0';
        printf("%s",outstr);
    }
    close(file);
    return 0;
}