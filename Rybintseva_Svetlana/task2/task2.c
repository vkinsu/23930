#include <stdio.h>
#include <time.h>
#include <stdlib.h>


int main(){
    time_t time_now;
    time(&time_now);
    putenv("TZ=America/Los_Angeles");
    printf("%s", ctime(&time_now));
    return 0;
}
