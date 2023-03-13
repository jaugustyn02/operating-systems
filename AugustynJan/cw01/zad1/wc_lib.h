#ifndef ZAD1_WC_LIB_H
#define ZAD1_WC_LIB_H

#define COMMAND_BUFF_SIZE 1024
#define TEMP_FILE_NAME "wc_lib.temp"

typedef struct Memory {
    char **tab; // array of pointers to memory blocks
    size_t size;  // num of currently occupied memory blocks
    size_t capacity; // max size
} Memory;

// 1) returns new initialized memory
Memory create(Memory *memory, size_t capacity);

// 2) runs wc on given file and adds output to new block memory
void add(Memory *memory, char* filename);

// 3) returns value of memory block at given index
char* get(Memory *memory, size_t index);

// 4) removes block of memory at given index
void remove(Memory *memory, size_t index);

// frees all blocks of memory
void free(Memory *memory);

// frees whole memory
void destruct(Memory *memory);

#endif //ZAD1_WC_LIB_H

