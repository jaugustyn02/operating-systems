#include "wc_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>


// 1) returns new initialized memory
Memory Memory_create(size_t capacity){
    Memory memory;
    memory.tab = calloc(capacity, sizeof (char*));
    memory.size = 0;
    memory.capacity = capacity;
    return memory;
}

// helper functions
long get_file_size(int fd){
    long f_size = lseek(fd, 0, SEEK_END);   // finding file length
    lseek(fd, 0, SEEK_SET);                 // fixing file pointer
    return f_size;
}

char* get_file_content(char* file_path){
    if(access(file_path, R_OK) != 0)
        return "";
    int fd = open(file_path, O_RDONLY);
    long f_size = get_file_size(fd);
    char* f_buff = calloc(f_size, sizeof(char));
    read(fd, f_buff, f_size);
    return f_buff;
}
// ends

// 2) runs wc on given file and saves result in a new memory block
void Memory_add(Memory *memory, char* filename){
    if(memory->size == memory->capacity) return;

    char *command = (char*)malloc(COMMAND_BUFF_SIZE * sizeof(char));
    sprintf(command, "wc -lwm %s 1> %s 2> /dev/null", filename, TEMP_FILE_FULL_PATH);
    system(command);

    memory->tab[memory->size] = get_file_content(TEMP_FILE_FULL_PATH);
    if(strlen(memory->tab[memory->size]) > 0)         // if content of a file is not empty (no error occurred)
        ++memory->size;

    sprintf(command, "rm -f %s 2> /dev/null", TEMP_FILE_FULL_PATH);
}

// 3) returns value of memory block at the given index
char* Memory_get(Memory *memory, size_t index){
    if(index >= memory->size) return "";
    return memory->tab[index];
}

// 4) removes memory block at the given index and shifts all the pointers on the right side one place towards left
void Memory_remove(Memory *memory, size_t index){
    if(index >= memory->size) return;

    for(int i = index+1; i < memory->size; i++)
        strcpy(memory->tab[i-1], memory->tab[i]);
    --memory->size;
    free(memory->tab[memory->size]);
    memory->tab[memory->size] = NULL;
}

// 5) frees all blocks of memory
void Memory_clear(Memory *memory){
    for(int i = 0; i < memory->capacity; i++){
        free(memory->tab[i]);
        memory->tab[i] = NULL;
    }
    free(memory->tab);
    memory->tab = NULL;
}
