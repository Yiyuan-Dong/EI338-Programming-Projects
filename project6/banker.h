#ifndef PROJECT6_BANKER_H
#define PROJECT6_BANKER_H

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define MAX_LINE 512

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);

void release_resources(int customer_num, int release[]);

int check_safety();

void read_maximum_request();

void output();

#endif //PROJECT6_BANKER_H
