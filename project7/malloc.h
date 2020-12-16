#ifndef PROJECT7_MALLOC_H
#define PROJECT7_MALLOC_H

#include <stdlib.h>

typedef struct struct_list_free {
    struct struct_list_free *next;
    int start;
    int end;
} free_node;

typedef struct struct_list_alloc {
    struct struct_list_alloc *next;
    char name[32];
    int start;
    int end;
} alloc_node;

free_node *build_free_node(int start, int end);

alloc_node *build_alloc_node(int start, int end, char *name);

void delete_node(void **last);

void insert_node(void **last, void **ptr);

#endif //PROJECT7_MALLOC_H
