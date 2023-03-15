#ifndef LIBWC_H
#define LIBWC_H

#include <stddef.h>

#define COMMAND_BUFF_SIZE 1024
#define TEMP_FILE_FULL_PATH "/tmp/wc_lib.temp"

typedef struct Memory {
    char **tab; // array of pointers to memory blocks
    size_t size;  // num of currently occupied memory blocks
    size_t capacity; // max size
} Memory;

Memory (*Memory_create) (size_t);
void (*Memory_add) (Memory*, char*);
char* (*Memory_get) (Memory*, size_t);
void (*Memory_remove) (Memory*, size_t);
void (*Memory_clear) (Memory*);


#endif // LIBWC_H