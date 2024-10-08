#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

void openFile(char *file) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        perror("Failed to open the file");
        printf("\n");
    } 
    else {
        printf("File was opened\n");
        fclose(f);
    }
}

void UserID() {
    printf ("Real User ID = %d\n",getuid());
    printf ("Effective User ID = %d\n\n",geteuid());
}

int main(){
    UserID();
    setuid(geteuid());
    UserID();

    char filename[4096];
    printf("Enter the name of the file:\n");
    fgets(filename,sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    openFile(filename);
    return 0;
}