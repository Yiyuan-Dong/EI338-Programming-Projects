#ifndef PROJECT5_4_BUFFER_H
#define PROJECT5_4_BUFFER_H

typedef int buffer_item;
#define BUFFER_SIZE 5

int insert_item(buffer_item item);

int remove_item(buffer_item *item);

void *producer(void *);

void *consumer(void *);

#endif //PROJECT5_4_BUFFER_H
