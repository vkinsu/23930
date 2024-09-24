#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    char* responce = malloc(255 * sizeof(char));

    time_t now = time(NULL);
    struct tm *local_time = gmtime(&now);

    local_time -> tm_hour -= 7;
    mktime(local_time);
    strftime(responce, 255, "%Y/%m/%d %H:%M:%S", local_time);
    
    printf(responce);
    printf(" (UTC) - time in California now\n");
    return 0;
}
