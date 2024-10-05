#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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

  char buf[1000];
  int line_number;
  printf("Enter number of line: ");
  scanf("%d", &line_number);
  while (line_number != 0) {
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
    printf("Enter number of line: ");
    scanf("%d", &line_number);
  }

  close(f);
  return 0;
}