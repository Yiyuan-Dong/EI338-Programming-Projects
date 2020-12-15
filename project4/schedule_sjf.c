#include "list.h"
#include "task.h"
#include "cpu.h"
#include <string.h>
#include <stdlib.h>

static struct node *head = NULL;

void add(char *name, int priority, int burst) {
    Task *task_ptr = malloc(sizeof(Task));
    task_ptr->name = (char *) malloc(strlen(name));
    strcpy(task_ptr->name, name);
    task_ptr->priority = priority;
    task_ptr->burst = burst;
    insert(&head, task_ptr);
}

Task *pick_nexttask() {
    if (head == NULL) {
        return NULL;
    }
    struct node *node_ptr = head->next;
    Task *s_task_ptr = head->task;
    while (node_ptr != NULL) {
        if (node_ptr->task->burst < s_task_ptr->burst) {
            s_task_ptr = node_ptr->task;
        }
        node_ptr = node_ptr->next;
    }
    delete(&head, s_task_ptr);
    return s_task_ptr;
}

void schedule() {
    Task *task_ptr;
    while ((task_ptr = pick_nexttask()) != NULL) {
        run(task_ptr, task_ptr->burst);
    }
}