#include <stdio.h>
#include <string.h>
#include "malloc.h"

alloc_node *alloc_head;
free_node *free_head;
int total_size;

free_node *build_free_node(int start, int end) {
    free_node *ptr = malloc(sizeof(free_node));
    ptr->start = start;
    ptr->end = end;
    ptr->next = NULL;
    return ptr;
}

alloc_node *build_alloc_node(int start, int end, char *name) {
    alloc_node *ptr = malloc(sizeof(alloc_node));
    ptr->start = start;
    ptr->end = end;
    ptr->next = NULL;
    strcpy(ptr->name, name);
    return ptr;
}

void delete_node(void **last) {
    void **ptr = (void **) *last;
    void *next = *ptr;
    *last = next;
    free((void *) ptr);
}

void insert_node(void **last, void **ptr) {
    void *next = *last;
    *ptr = next;
    *last = (void *) ptr;
}

void *traverse_list(void ***last, void ***curr) {
    *last = *curr;
    *curr = (void **) **curr;
    return *curr;
}

free_node *find_space_F(int space_size) {
    free_node *ptr = free_head;
    free_node *last;

    while (traverse_list((void ***) &last, (void ***) &ptr)) {
        if (ptr->end - ptr->start + 1 >= space_size) {
            return last;
        }
    }

    return NULL;
}

free_node *find_space_B(int space_size) {
    free_node *ptr = free_head;
    free_node *last;
    int best_num = 2147483647;
    free_node *best_last = NULL;
    while (traverse_list((void ***) &last, (void ***) &ptr)) {
        if (ptr->end - ptr->start + 1 >= space_size) {
            if (ptr->end - ptr->start + 1 - space_size < best_num) {
                best_num = ptr->end - ptr->start + 1 - space_size;
                best_last = last;
            }
        }
    }

    return best_last;
}


free_node *find_space_W(int space_size) {
    free_node *ptr = free_head;
    free_node *last;
    int worst_num = 0;
    free_node *worst_last = NULL;
    while (traverse_list((void ***) &last, (void ***) &ptr)) {
        if (ptr->end - ptr->start + 1 >= space_size) {
            if (ptr->end - ptr->start + 1 - space_size > worst_num) {
                worst_num = ptr->end - ptr->start - space_size + 1;
                worst_last = last;
            }
        }
    }

    return worst_last;
}

void insert_alloc(int start, int end, char *name) {
    alloc_node *ptr = alloc_head;
    alloc_node *last;

    while (traverse_list((void ***) &last, (void ***) &ptr)) {
        if (ptr->start > start) {
            break;
        }
    }

    insert_node((void **) last, (void **) build_alloc_node(start, end, name));
}


void cut_free_space(free_node *last, int space_size, char *name) {
    free_node *ptr = last->next;
    int alloc_start = ptr->start;
    int alloc_end = ptr->start + space_size - 1;
    int free_start = ptr->start + space_size;
    int free_end = ptr->end;

    delete_node((void **) last);
    if (free_start <= free_end) {
        insert_node((void **) last,
                    (void **) build_free_node(free_start, free_end));
    }

    insert_alloc(alloc_start, alloc_end, name);
}


alloc_node *find_by_name(char *name) {
    alloc_node *ptr = alloc_head;
    alloc_node *last;
    while (traverse_list((void ***) &last, (void ***) &ptr)) {
        if (!strcmp(ptr->name, name)) {
            return last;
        }
    }
    return NULL;
}


void release_node(alloc_node *alloc_last) {
    alloc_node *alloc_ptr = alloc_last->next;
    free_node *free_ptr = free_head;
    free_node *free_last;
    int free_end = alloc_ptr->end, free_start = alloc_ptr->start;

    while (traverse_list((void ***) &free_last, (void ***) &free_ptr)) {
        if (free_ptr->start > alloc_ptr->start) {
            break;
        }
    }

    if (free_ptr->start == free_end + 1) {
        free_end = free_ptr->end;
        delete_node((void **) free_last);
    }

    if (free_last->end == free_start - 1) {
        free_last->end = free_end;
    } else {
        insert_node((void **) free_last, (void **) build_free_node(free_start, free_end));
    }

    delete_node((void **) alloc_last);
}

void compact() {
    alloc_node *alloc_ptr = alloc_head, *alloc_last;

    int alloc_address_start = 0;
    while (traverse_list((void ***) &alloc_last, (void ***) &alloc_ptr)) {
        alloc_ptr->end = alloc_ptr->end - alloc_ptr->start + alloc_address_start;
        alloc_ptr->start = alloc_address_start;
        alloc_address_start = alloc_ptr->end + 1;
    }

    while (free_head->next) {
        delete_node((void **) free_head);
    }

    insert_node((void **) free_head,
                (void **) build_free_node(alloc_address_start, total_size - 1));
}

void show_stat() {
    free_node *free_ptr = free_head->next;
    alloc_node *alloc_ptr = alloc_head->next;

    while (free_ptr && alloc_ptr) {
        if (free_ptr->start < alloc_ptr->start) {
            printf("Address [%d:%d] Unused\n", free_ptr->start,
                   free_ptr->end);
            free_ptr = free_ptr->next;
        } else {
            printf("Address [%d:%d] Process %s\n", alloc_ptr->start,
                   alloc_ptr->end, alloc_ptr->name);
            alloc_ptr = alloc_ptr->next;
        }
    }

    while (free_ptr) {
        printf("Address [%d:%d] Unused\n", free_ptr->start,
               free_ptr->end);
        free_ptr = free_ptr->next;
    }

    while (alloc_ptr) {
        printf("Address [%d:%d] Process %s\n", alloc_ptr->start,
               alloc_ptr->end, alloc_ptr->name);
        alloc_ptr = alloc_ptr->next;
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Need argument!\n");
        return 0;
    }

    free_head = build_free_node(-2, -2);
    alloc_head = build_alloc_node(-2, -2, "");

    total_size = atoi(argv[1]);
    insert_node((void **) free_head,
                (void **) build_free_node(0, total_size - 1));

    char command[8], command_2[8], command_3[8];
    int argument, argument_2;

    while (1) {
        printf("allocator>");
        fflush(stdout);

        scanf("%s", command);

        if (!strcmp(command, "X")) {
            printf("Bye~~\n");
            break;
        }

        if (!strcmp(command, "RQ")) {
            scanf("%s %d %s", command_2, &argument, command_3);
            free_node *last = NULL;

            if (!strcmp(command_3, "W")) {
                last = find_space_W(argument);
            }
            if (!strcmp(command_3, "B")) {
                last = find_space_B(argument);
            }
            if (!strcmp(command_3, "F")) {
                last = find_space_F(argument);
            }
            if (!last) {
                printf("Error! No sufficient space or unexpected policy!\n");
                continue;
            }

            cut_free_space(last, argument, command_2);
            printf("Done!\n");

            continue;
        }

        if (!strcmp(command, "RL")) {
            scanf("%s", command);

            alloc_node *last = find_by_name(command);
            if (!last) {
                printf("Error! Name not found!\n");
                continue;
            }

            release_node(last);

            continue;
        }

        if (!strcmp(command, "C")) {
            compact();
            continue;
        }

        if (!strcmp(command, "STAT")) {
            show_stat();
            continue;
        }

        printf("?\n");
    }

    return 0;
}