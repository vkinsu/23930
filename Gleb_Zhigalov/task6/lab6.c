#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

int main() {
  char filename[257];
  printf("Write filename: ");
  scanf("%s", filename);
  size_t f = open(filename, O_RDONLY);
  if (f == -1) {
    printf("Reading is impossible\n");
    return -1;
  }

  int mas_shifts[1000];
  char symbol;
  int symbols_counter = 0;
  int amount_of_shifts = 1;
  mas_shifts[0] = 0;

  while(read(f, &symbol, 1)) {
    if (symbol == '\n') {
      mas_shifts[amount_of_shifts++] = symbols_counter;
    }
    symbols_counter++;
  }
  mas_shifts[amount_of_shifts++] = symbols_counter;

  printf("\n");
  printf("Shifts table:\n");
  for (int i = 1; i < amount_of_shifts - 1; i++) {
    printf("%d\n", mas_shifts[i] + 1);
  }
  printf("\n");

  printf("\n");
  printf("String lengths table:\n");
  printf("%d\n", mas_shifts[1] - mas_shifts[0]);
  for (int i = 2; i < amount_of_shifts; i++) {
    printf("%d\n", mas_shifts[i] - mas_shifts[i - 1] - 1);
  }
  printf("\n");

  char buf[1000];
  char number[20];
  int line_number = 1;

  fd_set set;
  struct timeval timer;
  while (line_number != 0) {
    printf("You have 5 seconds to enter number of line: ");
    fflush(stdout);
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set); // STDIN_FILENO - set of bits for descriptor
    timer.tv_sec = 5;
    timer.tv_usec = 0;

    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timer) == 0) {
      printf("\n");
      lseek(f, 0, SEEK_SET);
      while(read(f, &symbol, 1)) {
        write(1, &symbol, 1);
      }
      close(f);
      return 0;
    }

    
    scanf("%d", &line_number);
  
    if (line_number == 0) break;

    if (line_number < 1 || line_number > amount_of_shifts - 1) {
      printf("String with number %d does not exist\n", line_number);
    } else {
      if (line_number == 1) {
        lseek(f, mas_shifts[line_number - 1], SEEK_SET);
        read(f, buf, mas_shifts[line_number] - mas_shifts[line_number - 1]);
        write(1, buf, mas_shifts[line_number] - mas_shifts[line_number - 1]);
        printf("\n");
      } else {
        lseek(f, mas_shifts[line_number - 1] + 1, SEEK_SET);
        read(f, buf, mas_shifts[line_number] - mas_shifts[line_number - 1] - 1);
        write(1, buf, mas_shifts[line_number] - mas_shifts[line_number - 1] - 1);
        printf("\n");
      }
    }
  }
  

  close(f);
  return 0;
}