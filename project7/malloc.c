#include <stdio.h>
#include <string.h>
#include "malloc.h"

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


free_node *find_space_F(int space_size) {
    free_node *ptr = free_head->next;
    free_node *last = free_head;

    while (ptr != NULL) {
        if (ptr->end - ptr->start + 1 >= space_size) {
            return last;
        }
        ptr = ptr->next;
        last = last->next;
    }

    return NULL;
}

free_node *find_space_B(int space_size) {
    free_node *ptr = free_head->next;
    free_node *last = free_head;
    int best_num = 2147483647;
    free_node *best_last = NULL;
    while (ptr != NULL) {
        if (ptr->end - ptr->start + 1 >= space_size) {
            if (ptr->end - ptr->start + 1 - space_size < best_num) {
                best_num = ptr->end - ptr->start + 1 - space_size;
                best_last = last;
            }
        }
        ptr = ptr->next;
        last = last->next;
    }

    return best_last;
}


free_node *find_space_W(int space_size) {
    free_node *ptr = free_head->next;
    free_node *last = free_head;
    int worst_num = 0;
    free_node *worst_last = NULL;
    while (ptr != NULL) {
        if (ptr->end - ptr->start + 1 >= space_size) {
            if (ptr->end - ptr->start + 1 - space_size > worst_num) {
                worst_num = ptr->end - ptr->start - space_size + 1;
                worst_last = last;
            }
        }
        ptr = ptr->next;
        last = last->next;
    }

    return worst_last;
}

void insert_alloc(int start, int end, char *name) {
    alloc_node *ptr = alloc_head->next;
    alloc_node *last = alloc_head;

    while (ptr != NULL) {
        if (ptr->start > start) {
            break;
        }
        last = ptr;
        ptr = ptr->next;
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
    alloc_node *ptr = alloc_head->next;
    alloc_node *last = alloc_head;
    while (ptr != NULL) {
        if (!strcmp(ptr->name, name)) {
            return last;
        }
        last = ptr;
        ptr = ptr->next;
    }
    return NULL;
}

void insert_alloc_into_free(alloc_node *alloc_ptr) {
    free_node *last = free_head;
    free_node *ptr = free_head->next;

    while (ptr) {
        if (ptr->start > alloc_ptr->start) {
            break;
        }
        last = ptr;
        ptr = ptr->next;
    }

    insert_node((void **) last, (void **) build_free_node(alloc_ptr->start, alloc_ptr->end));
}

void release_node(alloc_node *last) {
    alloc_node *ptr = last->next;
    insert_alloc_into_free(ptr);
    delete_node((void **) last);
}

void compact() {
    free_node *last = free_head;
    free_node *ptr = free_head->next;

    while (ptr) {
        if (last->end + 1 == ptr->start) {
            last->end = ptr->end;
            ptr = ptr->next;
            delete_node((void **) last);
        } else {
            last = ptr;
            ptr = ptr->next;
        }
    }
}

void show_stat() {
    free_node *free_ptr = free_head->next;
    alloc_node *alloc_ptr = alloc_head->next;

    while (free_ptr && alloc_ptr) {
        if (free_ptr->start < alloc_ptr->start) {
            printf("Address [%d:%d] Unused\n", free_ptr->start, free_ptr->end);
            free_ptr = free_ptr->next;
        } else {
            printf("Address [%d:%d] Process %s\n", alloc_ptr->start, alloc_ptr->end,
                   alloc_ptr->name);
            alloc_ptr = alloc_ptr->next;
        }
    }

    while (free_ptr) {
        printf("Address [%d:%d] Unused\n", free_ptr->start, free_ptr->end);
        free_ptr = free_ptr->next;
    }

    while (alloc_ptr) {
        printf("Address [%d:%d] Process %s\n", alloc_ptr->start, alloc_ptr->end,
               alloc_ptr->name);
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

    int maximum = atoi(argv[1]);
    insert_node((void **) free_head,
                (void **) build_free_node(0, maximum - 1));

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