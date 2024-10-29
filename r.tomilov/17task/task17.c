#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 40
#define CTRL_D 4
#define CTRL_W 23
#define ERASE 127
#define KILL 21
#define CTRL_G 7

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
    char line[MAX_LINE_LENGTH + 1] = {0};
    int pos = 0;

    while (1) 
    {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) <= 0) 
        {
            perror("Ошибка при чтении ввода");
            break;
        }

        if (ch == CTRL_D && pos == 0) 
        {
            break;
        } 
        else if (ch == ERASE) 
        {
            if (pos > 0) 
            {
                pos--;
                printf("\b \b");
                fflush(stdout);
            }
        } 
        else if (ch == KILL) 
        {
            while (pos > 0) 
            {
                pos--;
                printf("\b \b");
            }
            fflush(stdout);
        } 
        else if (ch == CTRL_W) 
        {
            while (pos > 0 && line[pos - 1] == ' ') 
            {
                pos--;
                printf("\b \b");
            }
            while (pos > 0 && line[pos - 1] != ' ') 
            {
                pos--;
                printf("\b \b");
            }
            fflush(stdout);
        } 
        else if (ch == '\033')
        {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) break;
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) break;

            printf("^G");
            putchar(CTRL_G);
            fflush(stdout);
            pos += 2;
        }
        else if ((ch >= 1 && ch <= 31 && ch != CTRL_D && ch != CTRL_W && ch != ERASE && ch != KILL)) 
        {
            printf("^G");
            putchar(CTRL_G);
            fflush(stdout);
            pos += 2;
        } 
        else 
        {
            if (pos < MAX_LINE_LENGTH) 
            {
                line[pos++] = ch;
                printf("%c", ch);
                fflush(stdout);
            } 
            else if (ch == ' ') 
            {
                printf("\n");
                pos = 0;
                fflush(stdout);
            }
        }
    }
}

int main() 
{
    struct termios original_settings;

    set_noncanonical_mode(&original_settings);

    printf("Введите текст (CTRL-D для выхода):\n");
    fflush(stdout);

    handle_input();

    restore_terminal_mode(&original_settings);

    printf("\nЗавершение программы.\n");
    return 0;
}
