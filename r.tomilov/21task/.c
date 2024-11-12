#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios old_termios;

void handle_sigquit(int signum) {
    (void)signum;
    // ��������������� ������ ��������� ��������� ����� �������
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    exit(0);
}

int main() {
    struct termios new_termios;

    // ��������� ������ ��������� ���������
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    // ��������� ������ ���������� (CTRL-C)
    new_termios.c_cc[VINTR] = _POSIX_VDISABLE;

    // ������������� �������������� �����
    new_termios.c_lflag &= ~(ICANON | ECHO); // ��������� ������������ ����� � ���
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    // ������������� ���������� ������� SIGQUIT ��� ������
    struct sigaction sa_quit = {0};
    sa_quit.sa_handler = handle_sigquit;
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("������� CTRL-C ��� ��������� �������, CTRL-\\ ��� ������.\n");

    while (1) {
        char c = getchar();
        printf("%c", c); // ���������� �� %c

        // ���������, �������� �� �������� ������ CTRL-C
        if (c == 3) { // ASCII ��� ��� CTRL-C
            ioctl(STDOUT_FILENO, TIOCSBRK, 0);
            printf("Received CTRL-C.\n");
            fflush(stdout);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

    return 0;
}
