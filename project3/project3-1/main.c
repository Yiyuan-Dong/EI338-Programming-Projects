#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int value = 0;
int sudoku[9][9] = {};

void *check_block(void *param);

void *check_row(void *param);

void *check_column(void *param);

struct param_type {
    int i;
    int j;
    int *ret_addr;
};

int main(int argc, char *argv[]) {

    int fd = open("input.txt", O_RDONLY);
    dup2(fd, STDIN_FILENO);

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            scanf(" %d", ((int *)sudoku) + i * 9 + j);
        }
    }

    pthread_t tid[11];
    pthread_attr_t attr;
    int result[11] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    struct param_type params[9];

    pthread_attr_init(&attr);

    pthread_create(tid, &attr, check_column, (void *) result);
    pthread_create(tid + 1, &attr, check_row, (void *) (result + 1));
    for (int i = 0; i < 9; ++i) {
        params[i].i = i / 3;
        params[i].j = i % 3;
        params[i].ret_addr = result + 2 + i;
        pthread_create(tid + 2 + i, &attr, check_block, (void *) (params + i));
    }

    int sum = 0;

    for (int i = 0; i < 11; ++i) {
        pthread_join(tid[i], NULL);
        if (result[i] == 1){
            printf("Checkpoint %d: failed!\n", i + 1);
        }
        else{
            printf("Checkpoint %d: passed!\n", i + 1);
        }
        sum += result[i];
    }

    if (sum == 0) {
        printf("Valid sudoku!\n");
    }
    else{
        printf("Invalid sudoku!\n");
    }
    return 0;
}

void *check_block(void *param) {
    struct param_type *ptr = (struct param_type *) param;
    int base_i = ptr->i * 3;
    int base_j = ptr->j * 3;
    volatile int *ret_addr = ptr->ret_addr;
    int flag[10] = {};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            flag[sudoku[base_i + i][base_j + j]] += 1;
            if (flag[sudoku[base_i + i][base_j + j]] > 1) {
                *ret_addr = 1;
                return NULL;
            }
        }
    }
    *ret_addr = 0;
    return NULL;
}

void *check_row(void *param) {
    volatile int *ret_addr = (int *) param;
    for (int i = 0; i < 9; ++i) {
        int flag[10] = {};
        for (int j = 0; j < 9; ++j) {
            flag[sudoku[i][j]] += 1;
            if (flag[sudoku[i][j]] > 1) {
                *ret_addr = 1;
                return NULL;
            }
        }
    }
    *ret_addr = 0;
    return NULL;
}

void *check_column(void *param) {
    volatile int *ret_addr = (int *) param;
    for (int j = 0; j < 9; ++j) {
        int flag[10] = {};
        for (int i = 0; i < 9; ++i) {
            flag[sudoku[i][j]] += 1;
            if (flag[sudoku[i][j]] > 1) {
                *ret_addr = 1;
                return NULL;
            }
        }
    }
    *ret_addr = 0;
    return NULL;
}
