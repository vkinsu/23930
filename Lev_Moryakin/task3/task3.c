#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <ulimit.h>
#include <sys/resource.h>

#include <errno.h>

int main(int argc,char **argv){
    if(argc != 2){
        printf("команда принимает один аргумент - имя файла\n");
        return -1;
    }
    seteuid(5000);
    setlocale(LC_ALL,"rus");
    FILE *file;
    printf("Пользователь\n");
    printf("Реальный id: %d\n",getuid());
    printf("Эффективный id: %d\n",geteuid());
    file = fopen(argv[1],"r");
    if(file == NULL)perror(NULL);
    else fclose(file);
    int euid = geteuid();
    if(setuid(euid)==0)printf("\nchanged user id to %d\n",euid);
    printf("Пользователь\n");
    printf("Реальный id: %d\n",getuid());
    printf("Эффективный id: %d\n",geteuid());
    file = fopen(argv[1],"r");
    if(file == NULL)perror(NULL);
    else fclose(file);
    return 0;
}