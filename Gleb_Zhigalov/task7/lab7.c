#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/mman.h>

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
  size_t size = lseek(f, 0, SEEK_END);
  char *p = mmap(0, size, PROT_READ, MAP_SHARED, f, 0);

  while(symbols_counter < size) {
    if (*(p + symbols_counter) == '\n') {
      mas_shifts[amount_of_shifts++] = symbols_counter;
    }
    symbols_counter++;
  }
  mas_shifts[amount_of_shifts++] = symbols_counter;

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
      symbols_counter = 0;
      while(symbols_counter < size) {
        printf("%c", *(p + symbols_counter));
        symbols_counter++;
      }
      munmap(p, size);
      close(f);
      return 0;
    }
    int i;
    scanf("%d", &line_number);
  
    if (line_number == 0) break;

    if (line_number < 1 || line_number > amount_of_shifts - 1) {
      printf("String with number %d does not exist\n", line_number);
    } else {
      if (line_number == 1) {
        i = mas_shifts[line_number - 1];
      } else {
        i = mas_shifts[line_number - 1] + 1;
      }
      
      for (; i < mas_shifts[line_number]; i++) {
        printf("%c", *(p + i));
      }
      printf("\n");
    }
  }
  
  munmap(p, size);
  close(f);
  return 0;
}