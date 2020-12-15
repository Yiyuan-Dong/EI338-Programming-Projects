#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE_NUMBER_MASK 0x0000ff00
#define OFFSET_MASK 0x000000ff
#define PAGE_SIZE 256
#define TLB_NUM 16
#define FRAME_NUM 128
#define TABLE_ENTRY_NUM 256
#define MAX_LINE 32

typedef struct {
    int available;
    int physical_frame_id;
} table_entry;

typedef struct {
    int virtual_page_num;
    int physical_frame_id;
    int available;
} TLB_entry;

table_entry entry_list[TABLE_ENTRY_NUM];
char *frame_list[FRAME_NUM];
TLB_entry TLB[TLB_NUM];

int frame_index;
int TLB_index;
int TLB_hit_count, page_fault_count, total_count;

void read_frame(int page_num, char *buffer) {
    FILE *ptr = fopen("BACKING_STORE.bin", "r");
    int offset = PAGE_SIZE * page_num;
    fseek(ptr, offset, SEEK_SET);
    fread(buffer, PAGE_SIZE, 1, ptr);
}

int get_free_frame(){
    for (int i = 0; i < TABLE_ENTRY_NUM; ++i){
        if (entry_list[i].physical_frame_id == frame_index){
            entry_list[i].available = 0;
        }
    }

    for (int i = 0; i < TLB_NUM; ++i){
        if (TLB[i].physical_frame_id == frame_index){
            TLB[i].available = 0;
        }
    }

    int return_id = frame_index;
    frame_index = (frame_index + 1) % FRAME_NUM;
    return return_id;
}

char get_query_byte(int frame_id, int offset) {
    return *(frame_list[frame_id] + offset);
}

int get_frame_id(int page_num) {
    for (int i = 0; i < TLB_NUM; ++i) {
        if (TLB[i].available && TLB[i].virtual_page_num == page_num) {
            TLB_hit_count++;
            return TLB[i].physical_frame_id;
        }
    }

    table_entry *entry_ptr = entry_list + page_num;
    if (entry_ptr->available == 0) {
        page_fault_count++;

        int my_frame_id = get_free_frame();

        read_frame(page_num, frame_list[my_frame_id]);
        entry_ptr->available = 1;
        entry_ptr->physical_frame_id = my_frame_id;
    }

    TLB[TLB_index] = (TLB_entry) {page_num, entry_ptr->physical_frame_id, 1};
    TLB_index = (TLB_index + 1) % TLB_NUM;

    return entry_ptr->physical_frame_id;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Format: ./main {input_file}\n");
        return 0;
    }

    char buffer[MAX_LINE];
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    dup2(fd, STDOUT_FILENO);

    for (int i = 0; i < FRAME_NUM; ++i) {
        frame_list[i] = malloc(PAGE_SIZE);
    }

    FILE *input_file = fopen(argv[1], "r");
    while (fgets(buffer, MAX_LINE, input_file) != NULL) {

        total_count++;

        int query_address, page_num, offset, frame_id;
        sscanf(buffer, "%d\n", &query_address);
        page_num = (query_address & PAGE_NUMBER_MASK) >> 8;
        offset = query_address & OFFSET_MASK;
        frame_id = get_frame_id(page_num);

        printf("Virtual address: %d ", query_address);
        printf("Physical address: %d ", frame_id * PAGE_SIZE + offset);
        printf("Value: %d\n", get_query_byte(frame_id, offset));
    }

    printf("TLB-hit rate: %f\n", TLB_hit_count * (1.0) / total_count);
    printf("PAge-fault rate: %f\n",  page_fault_count * (1.0) / total_count);

    return 0;
}