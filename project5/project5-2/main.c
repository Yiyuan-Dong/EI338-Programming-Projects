#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#define NUMBER_OF_CHAIRS 3
#define NUMBER_OF_STUDENTS 10

pthread_mutex_t mutex;
sem_t sem;

sem_t *chair[NUMBER_OF_CHAIRS];
sem_t *office;

int begin;
int end;
int count;


void random_wait(int lower, int range){
    sleep(rand() % range + lower);
}

void *TA(void *ptr){

    sem_t *office_student;
    while (1) {
        sem_wait(&sem);

        /* Though read a pointer is atomic, here I still use a mutex,
         * the reasons are:
         * 1. to prevent the compiler from storing variable `office`
         *    in a register.
         * 2. to make my code more unterstandable and extendable.
         */

        pthread_mutex_lock(&mutex);

        office_student = office;

        pthread_mutex_unlock(&mutex);

        //helping the office student
        random_wait(1, 2);

        pthread_mutex_lock(&mutex);

        sem_post(office_student);
        count--;

        if (count > 0){
            office = chair[begin];
            begin = (begin + 1) % NUMBER_OF_CHAIRS;
        }

        pthread_mutex_unlock(&mutex);
    }
}

void *student(void *ptr){
    int flag = 0, my_number = (int)ptr;
    sem_t my_sem;
    sem_init(&my_sem, 0, 0);

    while(!flag){
        pthread_mutex_lock(&mutex);

        if (count < 4){
            count++;
            flag = 1;
            if (count == 1){
                office = &my_sem;
                printf("Student %d walk into the office...\n", my_number);
            }
            else{
                chair[end] = &my_sem;
                printf("Student %d sit on chair %d...\n", my_number, end + 1);
                end = (end + 1) % NUMBER_OF_CHAIRS;
            }
            sem_post(&sem);
        }
        else{
            printf("Student %d walk back...\n", my_number);
        }

        pthread_mutex_unlock(&mutex);

        if (flag){
            sem_wait(&my_sem);
        }
        else{
            random_wait(5, 10);
        }
    }

    printf("Student %d finished!\n", my_number);
    return NULL;
}


int main() {
    pthread_t pthread_students[NUMBER_OF_STUDENTS];
    pthread_t pthread_TA;

    printf("There are %d students!\n", NUMBER_OF_STUDENTS);
    srand((unsigned int)time((NULL)));

    pthread_create(&pthread_TA, NULL, TA, NULL);
    for (int i = 0; i < NUMBER_OF_STUDENTS; ++i){
        pthread_create(pthread_students + i, NULL, student, (void *)(i + 1));
    }

    for (int i = 0; i < NUMBER_OF_STUDENTS; ++i){
        pthread_join(pthread_students[i], NULL);
    }
    pthread_cancel(pthread_TA);
    pthread_join(pthread_TA, NULL);

    printf("All finished!\n");

    return 0;
}