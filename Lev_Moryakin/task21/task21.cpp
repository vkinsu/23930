#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int counter = 0;


void handINT(int riba){
    counter++;
    write(1,"\7",1);
    signal(SIGINT,handINT);
} 
void handQUIT(int riba){
    printf("\n%d times\n",counter);
    exit(0);
} 



int main(){
    signal(SIGINT,handINT);
    signal(SIGQUIT,handQUIT);

    while(1) { pause(); }
}