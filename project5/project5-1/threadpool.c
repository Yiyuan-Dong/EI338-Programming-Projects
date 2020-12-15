/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 20

#define TRUE 1

static pthread_mutex_t mutex;
static sem_t sem_empty, sem_full;
static int begin = 0, end = 0;

// this represents work that has to be 
// completed by a thread in the pool
typedef struct {
    void (*function)(void *p);

    void *data;
}
        task;

// the work queue
task worktodo[QUEUE_SIZE];

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) {
    sem_wait(&sem_full);

    pthread_mutex_lock(&mutex);

    worktodo[end] = t;
    end = (end + 1) % QUEUE_SIZE;
    sem_post(&sem_empty);

    pthread_mutex_unlock(&mutex);

    return 0;
}

// remove a task from the queue
task dequeue() {
    task ret_task;

    //sem_wait MUST be a cancellation point
    sem_wait(&sem_empty);

    pthread_mutex_lock(&mutex);

    ret_task = worktodo[begin];
    begin = (begin + 1) % QUEUE_SIZE;
    sem_post(&sem_full);

    pthread_mutex_unlock(&mutex);

    return ret_task;
}

// the worker thread in the thread pool
void *worker(void *param) {
    // execute the task
    task temp;

    while (1) {
        temp = dequeue();
        execute(temp.function, temp.data);
    }

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p) {
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p) {
    task submit_task;
    submit_task.function = somefunction;
    submit_task.data = p;

    enqueue(submit_task);

    return 0;
}

// initialize the thread pool
void pool_init(void) {
    sem_init(&sem_full, 0, 10);
    sem_init(&sem_empty, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        pthread_create(bee + i, NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void) {
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        pthread_cancel(bee[i]);
        pthread_join(bee[i], NULL);
    }
}
