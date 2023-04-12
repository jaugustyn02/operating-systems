#ifndef TOTAL_TIME_H
#define TOTAL_TIME_H

#include <stdbool.h>
#include <time.h>
#include <sys/resource.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

struct timespec start_time, end_time;
double elapsed_time;

double get_time(void){
    return (double) (end_time.tv_sec - start_time.tv_sec) +
                    (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;;
}

void time_start(void){
    clock_gettime(CLOCK_MONOTONIC, &start_time);
}

void time_end(void){
    clock_gettime(CLOCK_MONOTONIC, &end_time);
}

void print_time(void){
    printf("Elapsed time: %lf s\n", get_time());
}

int write_time(const char* filepath, char* top_text, char* bottom_text, bool append){
    FILE* file = fopen(filepath, (append==1 ? "a" : "w"));
    if(file == NULL){
        fprintf(stderr, "[rus_time.h] Error: Cannot create/open file: %s\n", filepath);
        return FAILURE;
    }
    if(strlen(top_text) > 0)
        fprintf(file, "%s\n", top_text);

    fprintf(file, "Elapsed time: %lf s\n", get_time());

    if(strlen(bottom_text) > 0)
        fprintf(file, "%s\n\n", bottom_text);
    else
        fprintf(file, "\n");

    fclose(file);
    return SUCCESS;
}

#endif // TOTAL_TIME_H