#include "list.h"
#include "task.h"
#include "cpu.h"
#include <string.h>
#include <stdlib.h>

static struct node *head = NULL;

void add(char *name, int priority, int burst){
    Task* task_ptr = malloc(sizeof(Task));
    task_ptr->name = (char *)malloc(strlen(name));
    strcpy(task_ptr->name, name);
    task_ptr->priority = priority;
    task_ptr->burst = burst;
    append(&head, task_ptr);
}

Task *pick_nexttask(){
    if (head == NULL){
        return NULL;
    }
    //struct node *node_ptr = head;
    Task* task_ptr = head->task;
    head = head->next;
    return task_ptr;
}

void schedule(){
    Task* task_ptr;
    while((task_ptr = pick_nexttask()) != NULL){
        if (task_ptr->burst <= 10) {
            run(task_ptr, task_ptr->burst);
        }
        else{
            run(task_ptr, 10);
            add(task_ptr->name, task_ptr->priority, task_ptr->burst - 10);
        }
    }
}