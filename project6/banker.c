#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "banker.h"

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int check_safety() {
    int satisfy[NUMBER_OF_CUSTOMERS] = {};
    int count = 0;
    int temp_available[NUMBER_OF_RESOURCES] = {};
    memcpy(temp_available, available, sizeof(available));

    while (1) {
        // we have complete all the process
        if (count == NUMBER_OF_CUSTOMERS) {
            return 0;
        }

        int modified = 0;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
            if (satisfy[i]) {
                continue;
            }

            int flag = 0;
            for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
                if (need[i][j] > temp_available[j]) {
                    flag = 1;
                    break;
                }
            }

            if (flag) {
                continue;
            } else {
                //that thread could complete and we should
                //recycle the resource
                for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
                    temp_available[j] += allocation[i][j];
                }
                count += 1;
                satisfy[i] = 1;
                modified = 1;
                break;
            }
        }

        if (!modified) {
            return -1;
        }
    }
}


void read_maximum_request() {
    int fd;
    char buf[MAX_LINE];
    FILE *ptr = fopen("input.txt", "r");

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
        for (int j = 0; j < NUMBER_OF_RESOURCES - 1; ++j) {
            fscanf(ptr, "%d,", ((int *) (maximum + i)) + j);
            need[i][j] = maximum[i][j];
        }
        // at the end of a line, there is no ','
        fscanf(ptr, "%d ", ((int *) (maximum + i)) + NUMBER_OF_RESOURCES - 1);
        need[i][NUMBER_OF_RESOURCES - 1] = maximum[i][NUMBER_OF_RESOURCES - 1];
    }
}


int request_resources(int customer_num, int request[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        if (request[i] > available[i] || request[i] > need[customer_num][i]) {
            return -1;
        }
    }

    // try to alloc resource for that process
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    //if the allocation is not safe, return the resource back
    if (check_safety()) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
            available[j] += request[j];
            allocation[customer_num][j] -= request[j];
            need[customer_num][j] += request[j];
        }
        return -1;
    }

    return 0;
}


int release_resources(int customer_num, int release[]) {
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        if (allocation[customer_num][j] < release[j]) {
            return -1;
        }
    }

    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        available[j] += release[j];
        allocation[customer_num][j] -= release[j];
        need[customer_num][j] += release[j];
    }

    return 0;
}


void output_matrix(char *name, int *matrix, int row_num, int column_num) {
    printf("%s\n", name);
    for (int i = 0; i < row_num; ++i) {
        for (int j = 0; j < column_num; ++j) {
            printf("%5d", matrix[i * column_num + j]);
        }
        printf("\n");
    }
}


void output() {
    output_matrix("Available", available, 1, NUMBER_OF_RESOURCES);
    output_matrix("Maximum", (int *) maximum, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    output_matrix("Allocation", (int *) allocation, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    output_matrix("Need", (int *) need, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
}


int main(int argc, const char *const *argv) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
        available[i] = (int) strtol(argv[i + 1], NULL, 10);
    }
    read_maximum_request();

    char command[16];
    int arguments[2 + NUMBER_OF_RESOURCES];
    while (1) {
        scanf("%s", command);

        if (!strcmp(command, "RQ")) {
            for (int i = 0; i < NUMBER_OF_RESOURCES + 1; ++i) {
                scanf("%d", arguments + i);
            }

            if (!request_resources(arguments[0], arguments + 1)) {
                printf("Done!\n");
            } else {
                printf("GG!\n");
            }
            continue;
        }

        if (!strcmp(command, "RL")) {
            for (int i = 0; i < NUMBER_OF_RESOURCES + 1; ++i) {
                scanf("%d", arguments + i);
            }

            if (!release_resources(arguments[0], arguments + 1)){
                printf("Done!\n");
            } else {
                printf("GG!\n");
            }
            continue;
        }

        if (!strcmp(command, "*")) {
            output();
            continue;
        }

        if (!strcmp(command, "quit")) {
            printf("Goodbye!\n");
            break;
        }

        printf("?\n");
    }

    return 0;
}