#ifndef RUS_TIME_H
#define RUS_TIME_H

#include <stdbool.h>
#include <time.h>
#include <sys/resource.h>

#define SUCCESS 0
#define FAILURE 1

clock_t ct_start=0, ct_end=0;
struct rusage ru_start, ru_stop;

double get_real_time(void){
    return (double) (ct_end - ct_start) / CLOCKS_PER_SEC;
}
double get_user_time(void){
    return (ru_stop.ru_utime.tv_sec - ru_start.ru_utime.tv_sec) +
                (ru_stop.ru_utime.tv_usec - ru_start.ru_utime.tv_usec) / 1.e6;
}
double get_system_time(void){
    return (ru_stop.ru_stime.tv_sec - ru_start.ru_stime.tv_sec) +
                  (ru_stop.ru_stime.tv_usec - ru_start.ru_stime.tv_usec) / 1.e6;
}

void time_start(void){
    ct_start = clock();
    getrusage(RUSAGE_SELF, &ru_start);
}

void time_end(void){
    ct_end = clock();
    getrusage(RUSAGE_SELF, &ru_stop);
}

void print_time(void){
    printf("Real time: %lf s\nUser time: %lf s\nSystem time: %lf s \n", get_real_time(), get_user_time(), get_system_time());
}

int write_time(const char* filepath, char* top_text, char* bottom_text, bool append){
    FILE* file = fopen(filepath, (append==1 ? "a" : "w"));
    if(file == NULL){
        fprintf(stderr, "[rus_time.h] Error: Cannot create/open file: %s\n", filepath);
        return FAILURE;
    }
    if(strlen(top_text) > 0)
        fprintf(file, "%s\n", top_text);

    fprintf(file, "Real time: %lf s\nUser time: %lf s\nSystem time: %lf s \n", get_real_time(), get_user_time(), get_system_time());

    if(strlen(bottom_text) > 0)
        fprintf(file, "%s\n\n", bottom_text);
    else
        fprintf(file, "\n");

    fclose(file);
    return SUCCESS;
}

#endif // RUS_TIME_H