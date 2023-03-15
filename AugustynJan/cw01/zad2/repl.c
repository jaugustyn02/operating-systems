#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#ifdef DYNAMIC
    #include "wc_lib_dynamic.h"
#else
    #include "wc_lib.h"
#endif

#define INPUT_BUFF_SIZE 1024
#define FILE_NAME_SIZE 256
#define ERROR_MSG_SIZE 256

char* error_msg;
bool stop_program = 0;

Memory memory;
bool array_initialized = 0;

clock_t ct_start, ct_end;
struct rusage ru_start, ru_stop;
// time measurement
//typedef struct{

    // 3 diff timers
    // 1 function to start watch
//     3 functions to end watch
//}Stopwatch;

typedef enum {
    init,       // init size - create new array with size 'size'
    count,      // count file - count lines, words and characters in the file and save result in array
    show,       // show index - prints saved result in array at the index
    delete,     // delete index - removes saved result in array at the index
    destroy,    // destroy - frees array
    q,          // quit program (exit)
    invalid     // invalid command - nothing happens
} Command_ID;

typedef struct {
    Command_ID cid;
    int num_arg;
    char* str_arg;
}Command;

Command parse_input(char* buff){
    // Parsing command
    Command command;
    const char corrector[] = " \n\t\r";
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
        strcpy(error_msg, "Invalid command");
        return command;
    }

    // checking that command not uses uninitialized array
    if(array_initialized == 0 && command.cid != init){
        strcpy(error_msg, "Array is not initialized");
        command.cid = invalid;
        return command;
    }
    // checking that array won't be initialized more than ones
    if(array_initialized == 1 && command.cid == init){
        strcpy(error_msg, "Array is already initialized");
        command.cid = invalid;
        return command;
    }
    // command destroy does not have any arguments
    if (command.cid == destroy)
        return command;

    // Parsing argument
    storage = strtok(NULL, corrector);
    if (storage == NULL){
        strcpy(error_msg, "No argument provided");
        command.cid = invalid;
        return command;
    }
    // command: count
    if (command.cid == count){
        command.str_arg = malloc((strlen(storage)+1) * sizeof(char));
        strcpy(command.str_arg, storage);
        return command;
    }
    // commands: init, show, delete
    int value = atoi(storage);
    if((value == 0 && storage[0] != '0') || value < 0){
        command.cid = invalid;
        strcpy(error_msg, "Invalid numeric argument");
        return command;
    }
    command.num_arg = value;
    return command;
}

void execute_command(Command command){
    switch (command.cid) {
        case init:
            memory = Memory_create((size_t)command.num_arg);
            array_initialized = 1;
            break;
        case count:
            Memory_add(&memory, command.str_arg);
            break;
        case show:
            printf("%s", Memory_get(&memory, (size_t)command.num_arg));
            break;
        case delete:
            Memory_remove(&memory, (size_t)command.num_arg);
            break;
        case destroy:
            Memory_clear(&memory);
            array_initialized = 0;
            break;
        case q:
            printf("<< Program ends\n");
            stop_program = 1;
            if(array_initialized == 1)
                Memory_clear(&memory);
            break;
        case invalid:
            printf("<< Error: %s\n", error_msg);
            break;
    }
}

void print_times(){
    double real_time, user_time, system_time;

    real_time = (double) (ct_end - ct_start) / CLOCKS_PER_SEC;
    user_time = (ru_stop.ru_utime.tv_sec - ru_start.ru_utime.tv_sec) +
            (ru_stop.ru_utime.tv_usec - ru_start.ru_utime.tv_usec) / 1.e6;
    system_time = (ru_stop.ru_stime.tv_sec - ru_start.ru_stime.tv_sec) +
             (ru_stop.ru_stime.tv_usec - ru_start.ru_stime.tv_usec) / 1.e6;

    printf("<< Times: Real=%lf s | User=%lf s | System=%lf s \n", real_time, user_time, system_time);
}

int main(int argc, char **argv){
    Command command;
    error_msg = malloc(ERROR_MSG_SIZE * sizeof(char));
    char* input_buff = malloc(INPUT_BUFF_SIZE * sizeof(char));
    do{
        printf(">>>> ");
        fgets(input_buff, INPUT_BUFF_SIZE, stdin);

        ct_start = clock();      // clock time start
        getrusage(RUSAGE_SELF, &ru_start);

        if (strspn(input_buff, " \r\n\t") == strlen(input_buff))    // buffer contains only whitespace characters
            continue;
        command = parse_input(input_buff);
        execute_command(command);

        ct_end = clock();        // clock time end
        getrusage(RUSAGE_SELF, &ru_stop);
        print_times();
    }while(stop_program == 0);
    return 0;
}