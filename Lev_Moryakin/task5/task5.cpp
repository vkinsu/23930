#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <regex>
#include <string>
#include <iostream>
using namespace std;

bool InpCheck(string str);

int main(int argc,char **argv){
    char str[1];
    char *outstr;
    if(argc != 2){
        printf("команда принимает один аргумент - имя файла\n");
        return -1;
    }
    int file = open(argv[1],O_RDONLY);
    if(file == -1){
        printf("не удалось открыть файл %s\n",argv[1]);
        return -1;
    }
    int bytesread = 0;
    int *newlines = (int*)malloc(sizeof(int) * 1);
    newlines[0] = 0;
    int newlinessize = 1;
    int iline = 1;
    int showline;
    string inpshowline;
    while(read(file,str,1)){
        //printf("%c ",str[0]);
        if(str[0] == '\n'){
            if(newlinessize <= iline + 1){
                newlinessize *= 2;
                newlines = (int*)realloc(newlines, newlinessize * sizeof(int));
            }
            newlines[iline++] = bytesread+1; //позиция переноса строки в файле
        }
        bytesread++;
    }
    printf("в файле %d строк\n",iline - 1);
    //printf("\n");
    //for(int i = 0; i < iline; i++) printf("%d\n",newlines[i]);
    while(1){
        cin >> inpshowline;
        if(!InpCheck(inpshowline)){
            printf("некорректный формат числа\n");
            continue;
        }
        showline = stoi(inpshowline);
        if(showline == 0)return 0;
        if(showline <= 0 || showline >= iline){
            printf("в файле отсутствует строка с номером %d\n",showline);
            continue;
        }
        printf("%d-%d\n",newlines[showline - 1],newlines[showline]);
        lseek(file,newlines[showline - 1],SEEK_SET);
        outstr = (char*)malloc(newlines[showline] - newlines[showline - 1] + 1);
        bytesread = read(file,outstr,newlines[showline] - newlines[showline - 1]);
        outstr[bytesread] = '\0';
        printf("%s",outstr);
    }
    close(file);
    return 0;
}

bool InpCheck(string str){
    regex riba("[1-9]{1}[0-9]{0,9}");
    regex stop("0{1}");
    //regex riba("[1-9]{1}[0-9]{,9}");
    smatch m;
    return regex_match(str,m,riba) || regex_match(str,m,stop);
}