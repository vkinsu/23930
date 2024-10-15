#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>

int fd;

void timeout_handler(int sig) {
    char c;
    printf("\nTime is up!\n\n");
    lseek(fd, 0L, SEEK_SET); // Возвращаем указатель в начало файла
    while (read(fd, &c, 1)) { // Читаем файл символ за символом
        write(1, &c, 1); // Печатаем на экран
    }
    exit(0); // Завершаем программу после вывода содержимого файла
}

int main(int argc, char *argv[]) {
    long lines_pos[500];
    int i = 1, j = 0, line_number, line_ln[500];
    char c, buf[300];
    
    // Открытие файла
    if(( fd =  open(argv[1], O_RDONLY)) == -1) {
        printf("Failed to open file.\n");
        exit(1);
    }
    
    lines_pos[1] = 0L; // Первая строка начинается с начала файла
    
    // Чтение файла и определение позиций строк
    while(read(fd, &c, 1)) {
        if(c == '\n') {
            j++;
            line_ln[i++] = j;
            lines_pos[i] = lseek(fd, 0L, 1); // Сохраняем позицию следующей строки
            
            // Выводим информацию о строке для тестирования
            lseek(fd, lines_pos[i - 1], SEEK_SET); 
            read(fd, buf, line_ln[i - 1]);
            buf[line_ln[i - 1] - 1] = '\0';
            printf("%ld  %s  %d\n", lines_pos[i - 1], buf, line_ln[i-1]-1);
            
            j = 0;
        } else {
            j++;
        }
    }

    // Установка обработчика сигнала для таймера
    signal(SIGALRM, timeout_handler);

    // Основной цикл программы
    while(1) {
        printf("Enter line number. You have 5 seconds: ");
        alarm(5); // Устанавливаем таймер на 5 секунд
        
        if (scanf("%d", &line_number) == 1) {
            alarm(0); // Сбрасываем таймер при успешном вводе
            if(line_number == 0)
                exit(0); // Завершение программы при вводе 0
            
            lseek(fd, lines_pos[line_number], 0); // Переход к нужной строке
            
            if(read(fd, buf, line_ln[line_number])) {
                write(1, buf, line_ln[line_number]); // Вывод строки
            } else {
                printf("Invalid line number\n"); // Сообщение об ошибке при неверном номере строки
            }
        } else {
            alarm(0); // Сбрасываем таймер при ошибке ввода
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Очищаем буфер ввода
        }
    }
}
