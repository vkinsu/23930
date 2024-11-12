#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_C 3
#define CTRL_G 7
#define CTRL_BACKSLASH 28

int ctrl_c_count = 0;

void set_noncanonical_mode(struct termios *original) 
{
    struct termios new_settings;

    if (tcgetattr(STDIN_FILENO, original) == -1) 
    {
        perror("Ошибка: не удалось получить атрибуты терминала");
        exit(1);
    }
    
    new_settings = *original;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    new_settings.c_lflag &= ~ISIG;
    
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) == -1) 
    {
        perror("Ошибка: не удалось установить неканонический режим терминала");
        exit(1);
    }
}

void restore_terminal_mode(struct termios *original) 
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, original) == -1) 
    {
        perror("Ошибка: не удалось восстановить режим терминала");
    }
}

void handle_input() 
{
    int pos = 0;

    while (1) 
    {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) <= 0) 
        {
            perror("Ошибка при чтении ввода");
            break;
        }

        if (ch == CTRL_C) 
        {
            ctrl_c_count++;
            printf("^G");
            putchar(CTRL_G);
            fflush(stdout);
            pos += 2;
        } 
        else if (ch == CTRL_BACKSLASH) 
        {
            printf("\nCtrl-C был нажат %d раз\n", ctrl_c_count);
            fflush(stdout);
            break;
        }
    }
}

int main() 
{
    struct termios original_settings;

    set_noncanonical_mode(&original_settings);

    printf("Введите текст (CTRL-\\ для завершения программы):\n");
    fflush(stdout);

    setvbuf(stdout, NULL, _IONBF, 0);
    handle_input();

    restore_terminal_mode(&original_settings);

    printf("\nЗавершение программы.\n");
    return 0;
}
