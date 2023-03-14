#include "wc_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <cstring>
#include <cstdio>
#include <string.h>

#define INPUT_BUFF_SIZE 1024

char* ERROR_MSG = malloc(256 * sizeof(char));
bool STOP_PROGRAM = 0;

Memory memory;
bool array_is_initialized = 0;

typedef enum {
    init,       // init size - create new array with size 'size'
    count,      // count file - count lines, words and characters in the file and save result in array
    show,       // show index - prints saved result in array at the index
    delete,     // delete index - removes saved result in array at the index
    destroy,    // destroy - frees array
    q           // quit program (exit)
    invalid     // invalid command - nothing happens
} Command_ID;

typedef struct {
    Command_ID cid;
    int num_arg;
    char* str_arg;
}Command;

Command parse_input(char* buff){
    Command command;
    const char corrector[] = " ";
    char *storage;

    storage = strtok(buff, corrector);
    if (strcmp(storage, "init") == 0){
        command.cid = init;
    } else if(strcmp(storage, "count") == 0){
        command.cid = count;
    } else if(strcmp(storage, "show") == 0){
        command.cid = show;
    } else if(strcmp(storage, "delete") == 0){
        command.cid = delete;
    } else if(strcmp(storage, "destroy") == 0){
        command.cid = destroy;
    } else if(strcmp(storage, "q") == 0){
        command.cid = q;
        return command;
    } else{
        command.cid = invalid;
        strcpy(ERROR_MSG, "Invalid command")
        return command;
    }

    // checking if command uses not initialized array
    if(array_is_initialized == 0 && command.cid != init){
        strcpy(ERROR_MSG, "Array is not initialized")
    }
    // checking if array won't

    storage = strtok(NULL, corrector);
    if (strlen(storage) == 0){
        strcpy(ERROR_MSG, "No argument provided")
    }
    if (command.cid == count){
        command.str_arg = malloc((strlen(buff)+1) * sizeof(char));
        strcpy(command.str_arg, buff);
        return command;
    }
    // command.cid is in {init, show, delete}
    int value = atoi(storage);
    if((value == 0 && storage[0] != '0') || value < 0){
        command.cid = invalid;
        strcpy(ERROR_MSG, "Invalid numeric argument")
        return command;
    }
    command.num_arg = value;
    return command;
}

void execute_command(Command command){
    switch (command.cid) {
        case init:
            break;
        case count:
            break;
        case show:
            break;
        case delete:
            break;
        case destroy:
            if(array_is_initialized == 1){
                Memory_clear(&memory);
                array_is_initialized = 0;
            }
            else{
                strcpy(ERROR_MSG, "Array is not initialized")
            }
            break;
        case q:
            quit = 1;
            return;

    }
}

//// time measurement
//struct StopWatch{
//    // 3 diff timers
//    // 1 function to start watch
//    // 3 functions to end watch
//}StopWatch;

int main(int argc, char **argv){
    Command command;
    char* input_buff = malloc(INPUT_BUFF_SIZE * sizeof(char));
    do{
        fgets(input_buff, INPUT_BUFF_SIZE, stdin);
        command = parse_input(input_buff);


    }while(quit == 0);

    return 0;
}