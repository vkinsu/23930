#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    time_t now;
    (void)time(&now);
    putenv("TZ=PST8PDT");
    printf("%s", ctime(&now));
    exit(0);
}