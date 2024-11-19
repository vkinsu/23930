#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define KIIL_STR "\r\033[K"
#define ERASE "\b \b"
#define BEEP "\x07"

int main()
{
    char buffer[41];
    int buffer_index = 0;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    int index = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
    newt.c_cc[VERASE] = 8;   // Backspace
    newt.c_cc[VWERASE] = 23; // Ctrl+W
    newt.c_cc[VKILL] = 21;   // Ctrl+U
    newt.c_cc[VEOF] = 4;     // Ctrl+D

    while (1)
    {
        char c;
        read(STDIN_FILENO, &c, 1);
        if (c == 10)
        {
            buffer_index = 0;
            write(1, &c, 1);
            break;
        }
        if (c == 4 && buffer_index == 0)
            break;
        if (c == 127)
        {
            if (buffer_index > 0)
            {
                buffer_index--;
                write(1, ERASE, strlen(ERASE));
                continue;
            }
        }
        else if (c == 21)
        {
            write(1, KIIL_STR, strlen(KIIL_STR));
            buffer_index = 0;
            continue;
        }
        else if (c == 23)
        {
            while (buffer_index > 0 && buffer[buffer_index - 1] == ' ')
            {
                buffer_index--;
                write(1, ERASE, strlen(ERASE));
            }
            while (buffer_index > 0 && buffer[buffer_index - 1] != ' ')
            {
                buffer_index--;
                write(1, ERASE, strlen(ERASE));
            }
            continue;
        }
        else if (buffer_index < 40 && (c >= 32 && c <= 126))
        {
            buffer[buffer_index++] = c;
            write(1, &c, 1);
        }
        else if (c >= 32 && c <= 126)
        {
            buffer_index = 0;
            write(1, "\n", 1);
            buffer[buffer_index++] = c;
            write(1, &c, 1);
        }
        else
        {
            write(1, BEEP, strlen(BEEP));
            continue;
        }
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
    return 0;
}
