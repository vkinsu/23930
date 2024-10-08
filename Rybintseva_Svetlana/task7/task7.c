#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>

#define ARR_START_CAPACITY 1024
#define BUFF_SIZE 1024
#define TIMEOUT 5

typedef struct Dinamic_arr {
    int arr_capacity;
    int arr_size;
    int *arr;
} Dinamic_arr;

Dinamic_arr* init_arr() {
    Dinamic_arr *arr = malloc(sizeof(Dinamic_arr));
    arr->arr_capacity = ARR_START_CAPACITY;
    arr->arr_size = 0;
    arr->arr = malloc(sizeof(int) * arr->arr_capacity);
    return arr;
}

void free_arr(Dinamic_arr *arr) {
    free(arr->arr);
    free(arr);
}

void push_to_arr(Dinamic_arr *arr, int offset) {
    if (arr->arr_size == arr->arr_capacity) {
        int *new_arr = malloc(sizeof(int) * arr->arr_capacity * 2);
        arr->arr_capacity *= 2;
        memcpy(new_arr, arr->arr, sizeof(int) * arr->arr_size);
        free(arr->arr);
        arr->arr = new_arr;
    }
    arr->arr[arr->arr_size] = offset;
    arr->arr_size += 1;
}

void print_mapped_str(char *mapped, Dinamic_arr* arr, int idx){
    for(int i=arr->arr[idx]; i<arr->arr[idx+1]; i++){
        printf("%c", mapped[i]);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        perror("error: no input file");
        return 1;
    }

    int input_file = open(argv[1], O_RDONLY);
    // int input_file = open("./text1.txt", O_RDONLY);

    if (input_file == -1) {
        perror("error: can't open file");
        return 1;
    }


    struct stat f_stat;
    if (fstat(input_file, &f_stat) == -1) {
        perror("error: can't get file size");
        close(input_file);
        return 1;
    }

    size_t filesize = f_stat.st_size;

    char *mapped = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, input_file, 0);
    if (mapped == MAP_FAILED) {
        perror("error: mapping file");
        close(input_file);
        return 1;
    }


    Dinamic_arr *arr = init_arr();
    push_to_arr(arr, 0);
    int str_len = 0;
    for (int i=0; i<filesize; i++){
        if (mapped[i] == 0x0A){
            push_to_arr(arr, arr->arr[arr->arr_size - 1] + str_len + 1);
            str_len = -1;
        }
        str_len++;
    }
    arr->arr_size--;



    fd_set readfds;
    struct timeval timeout;
    int number;

    // Настраиваем таймаут
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    // Настраиваем набор дескрипторов для select
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    printf("Введите число (у вас есть %d секунд): ", TIMEOUT);
    fflush(stdout); // Принудительно сбрасываем буфер вывода иначе сразу не выведет, будет ждать таймаута

    // Ожидаем ввода или истечения времени
    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

    if (result == -1) {
        perror("error: select");
    } else if (result == 0) {
        // Таймаут истек, выводим весь файл
        printf("\n");
        for(int i=0; i<arr->arr_size; i++){
            print_mapped_str(mapped, arr, i-1);
        }
    } else {
        // Ввод доступен
        int str_num;
        scanf("%d", &str_num);
        print_mapped_str(mapped, arr, str_num-1);
    }
 

    if (munmap(mapped, filesize) == -1) {
        perror("error: unmapping file");
    }


    free_arr(arr);
    close(input_file);
    return 0;
}
