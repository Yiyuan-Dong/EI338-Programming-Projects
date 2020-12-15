#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUMBER_OF_PHILOSOPHERS 5

pthread_mutex_t mutex[NUMBER_OF_PHILOSOPHERS];

void pickup_forks(int phi_number){
    // different philosophers pick chopsticks
    // in different order to prevent deadlock
    if (phi_number % 2 == 0){
        pthread_mutex_lock(mutex + phi_number);
        pthread_mutex_lock(mutex + (phi_number + 1) % NUMBER_OF_PHILOSOPHERS);
    }
    else{
        pthread_mutex_lock(mutex + (phi_number + 1) % NUMBER_OF_PHILOSOPHERS);
        pthread_mutex_lock(mutex + phi_number);
    }
}

void return_forks(int phi_number){
    pthread_mutex_unlock(mutex + phi_number);
    pthread_mutex_unlock(mutex + (phi_number + 1) % NUMBER_OF_PHILOSOPHERS);
}

void *philosopher(void* ptr){
    int my_number = (int)ptr;
    pickup_forks(my_number);
    sleep(2);
    printf("Philosopher %d finished eating!\n", my_number);
    return_forks(my_number);
    return NULL;
}

int main(){
    pthread_t  phi_pid[NUMBER_OF_PHILOSOPHERS];

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i){
        pthread_mutex_init(mutex + i, NULL);
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i){
        pthread_create(phi_pid + i, NULL, philosopher, (void *)i);
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i){
        pthread_join(phi_pid[i], NULL);
    }

    return 0;
}