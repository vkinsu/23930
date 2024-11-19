#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define SL 7

int main(){
    int descriptors[2];
    pipe(descriptors);
    if(fork() == 0){
        //дитя
        char incoming[SL];
        read(descriptors[0],incoming,SL);
        printf("[%d]",getpid());
        for(int i = 0; i < SL; i++)
            printf("%c",toupper(incoming[i]));
        printf("\n");
    }
    else{
        char string[SL] = "yariba";
        printf("[%d]%s\n",getpid(),string);
        write(descriptors[1],string,SL);
    }
    return 0;
}