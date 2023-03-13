#include <stddef.h>

#ifndef ZAD1_WC_LIB_H
#define ZAD1_WC_LIB_H

#define COMMAND_BUFF_SIZE 1024
#define TEMP_FILE_FULL_PATH "/tmp/wc_lib.temp"

typedef struct Memory {
    char **tab; // array of pointers to memory blocks
    size_t size;  // num of currently occupied memory blocks
    size_t capacity; // max size
} Memory;

// 1) returns new initialized memory
Memory Memory_create(size_t capacity);

// 2) runs wc command on the given file and saves output in a new block memory
void Memory_add(Memory *memory, char* filename);

// 3) returns value of memory block at the given index
char* Memory_get(Memory *memory, size_t index);

// 4) removes block of memory at the given index
void Memory_remove(Memory *memory, size_t index);

// frees all blocks of memory
void Memory_clear(Memory *memory);

#endif //ZAD1_WC_LIB_H

