#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	printf("Real: %d\n", getuid());
	printf("Eff: %d\n", geteuid());
	FILE *fp;
	if ((fp=fopen("file", "r")) == NULL){
		perror("Error opening file");
		return -1;
	}
	fclose(fp);
	setuid(getuid());
	printf("Real: %d\n", getuid());
	printf("Eff: %d\n", geteuid());
	if ((fp=fopen("file", "r")) == NULL){
		perror("Error opening file");
		return -1;
	}
	fclose(fp);
	return 0;
}

