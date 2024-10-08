#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define ARR_START_CAPACITY 1024
#define BUFF_SIZE 1024

typedef struct Dinamic_arr{
    int arr_capacity;
    int arr_size;
    int *arr;
}Dinamic_arr;

Dinamic_arr* init_arr(){
    Dinamic_arr *arr = malloc(sizeof(Dinamic_arr));
    arr->arr_capacity = ARR_START_CAPACITY;
    arr->arr_size = 0;
    arr->arr = malloc(sizeof(int)*arr->arr_capacity);
    return arr;
}

void free_arr(Dinamic_arr *arr){
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

int main(int argc, char* argv[]){
    if (argc == 1){
        perror("error: no input file");
        return 1;
    }

    int input_file = open(argv[1], O_RDONLY);
    // int input_file = open("/mnt/d/documents/os/task5/text1.txt", O_RDONLY);
    if (input_file == -1){
        perror("error: can't open file");
        return 1;
    }

    Dinamic_arr *arr = init_arr();
    push_to_arr(arr, 0);
    
    char buff[BUFF_SIZE+1];
    int result = 1;
    int str_len = 0;
    while (result != 0){
        result = read(input_file, buff, sizeof(buff) - 1);
        if (result == -1){
            close(input_file);
            perror("error: can't read file");
            return 1;
        }
        for (int i=0; i<result; i++){
            if (buff[i] == 0x0A){
                push_to_arr(arr, arr->arr[arr->arr_size - 1] + str_len + 1);
                str_len = -1;
            }
            str_len++;
        }
    }
    arr->arr_size--;
    
    // for (int i=0; i<arr->arr_size; i++){
    //     printf("%d %d\n", i, arr->arr[i]);
    // }

    int str_num;
    printf("enter string number: ");
    scanf("%d", &str_num);
    str_num--;
    if (str_num < 0 || str_num >= arr->arr_size){
        printf("incorrect number\n");
        free_arr(arr);
        close(input_file);
        return 1;
    }

    off_t newPosition = lseek(input_file, arr->arr[str_num], SEEK_SET);
    result = 1;
    while (result != 0){
        result = read(input_file, buff, sizeof(buff) - 1);
        if (result == -1){
            close(input_file);
            perror("error: can't read file");
            return 1;
        }
        for (int i=0; i<result; i++){
            if (buff[i] == 0x0A){
               break;
            }
            printf("%c", buff[i]);
        }
    }
    printf("\n");

    free_arr(arr);
    close(input_file);
    return 0;
}