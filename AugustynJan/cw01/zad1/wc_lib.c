#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wc_lib.h"

#define TEMP_DIR_PATH "/tmp/"

// returns new initialized memory
Memory create(Memory *memory, size_t capacity){
    Memory *mem = malloc(sizeof (Memory));
    mem->tab = calloc(capacity, sizeof (char*));
    mem->size = 0;
    mem->capacity = capacity;
    return mem;
}

// runs wc on given file and adds output to new block memory
void add(Memory *mem, char* filename){
    char *command = (char*)malloc(COMMAND_BUFF_SIZE * sizeof(char));
    sprintf(command, "wc -lwm %s 1> %s%s 2> /dev/null", filename, , TEMP_DIR_PATH, TEMP_FILE_NAME);
    system(command);
    sprintf(command, "rm -f %s%s 2> /dev/null", TEMP_DIR_PATH, TEMP_FILE_NAME);
}

// returns value of memory block at given index
char* get(Memory *mem, size_t index){
    return *(mem->tab+index);
}

// removes block of memory at given index
void remove(Memory *mem, size_t index){

}

// frees block of memory at given index
void free(Memory *mem){

}

// destructs
void destruct(Memory *mem){

}