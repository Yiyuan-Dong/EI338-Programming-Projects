#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "buffer.h"

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t sem_empty, sem_full;
int begin = 0, end = 0;

int get_rand_num(int rand_range){
    int number = rand() % rand_range + 1;
    return number;
}

int insert_item(buffer_item item){
    sem_wait(&sem_full);

    pthread_mutex_lock(&mutex);

    buffer[end] = item;
    end = (end + 1) % BUFFER_SIZE;
    sem_post(&sem_empty);

    pthread_mutex_unlock(&mutex);

    return 0;
}

int remove_item(buffer_item* item){
    sem_wait(&sem_empty);

    pthread_mutex_lock(&mutex);

    *item = buffer[begin];
    begin = (begin + 1) % BUFFER_SIZE;
    sem_post(&sem_full);

    pthread_mutex_unlock(&mutex);

    return 0;
}

void *producer(void *param){
    buffer_item item;
    int my_number = (int)param;

    while(1){
        sleep(get_rand_num(3));

        item = get_rand_num(100);
        if(insert_item(item)){
            printf("Report error condition!\n");
        }
        else{
            printf("producer %d produced %d\n", my_number, item);
        }
    }

    return NULL;
}

void *consumer(void *param){
    buffer_item item;
    int my_number = (int)param;

    while(1){
        sleep(get_rand_num(3));

        if(remove_item(&item)){
            printf("Report error condition!\n");
        }
        else{
            printf("consumer %d consumed %d\n", my_number, item);
        }
    }

    return NULL;
}


int main(int argc, char *argv[]) {
    pthread_t producer_pid[100];
    pthread_t consumer_pid[100];

    if (argc < 4){
        printf("Need more argument!\n");
        return 0;
    }

    srand((unsigned int)time(NULL));

    int time_sleep = atoi(argv[1]);
    int num_producer = atoi(argv[2]);
    int num_consumer = atoi(argv[3]);

    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_empty, 0, 0);
    sem_init(&sem_full, 0, BUFFER_SIZE);

    for(int i = 0; i < num_producer; ++i){
        pthread_create(producer_pid + i, NULL, producer, (void *)(i + 1));
    }
    for (int i = 0; i < num_consumer; ++i){
        pthread_create(consumer_pid + i, NULL, consumer, (void *)(i + 1));
    }

    sleep(time_sleep);

    for(int i = 0; i < num_producer; ++i){
        pthread_cancel(producer_pid[i]);
        pthread_join(producer_pid[i], NULL);
    }
    for (int i = 0; i < num_consumer; ++i){
        pthread_cancel(consumer_pid[i]);
        pthread_join(consumer_pid[i], NULL);
    }

    return 0;
}
