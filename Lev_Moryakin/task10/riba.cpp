#include <stdio.h>
int main(int argc,char** argv){
    using namespace std;
    printf("Hi! My name is riba and my arguments are:\n");
    for(int i = 0; argv[i] != NULL; i++)printf("%s\n",argv[i]);
    printf("end of output\n");
    return argc;
}