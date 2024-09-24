#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    time_t now;

    time(&now);
    putenv("TZ=America/Los_Angeles");
    printf("%s", ctime(&now));
    return 0;
}