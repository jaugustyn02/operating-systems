#include <stdio.h>
#include <stdlib.h>
#include "total_time.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <rectangle width> <number of programs>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    double a = 0, b = 1;
    double width = atof(argv[1]);
    if (width > b - a){
        printf("Error: Rectangle width cannot be greater than integration interval\n");
        exit(EXIT_FAILURE);
    }
    int num_programs = atoi(argv[2]);
    if (num_programs < 1){
        printf("Error: Number of programs must be greater or equal 1\n");
        exit(EXIT_FAILURE);
    }

    time_start();

    const char *pipe_name = "sums_pipe";
    mkfifo(pipe_name, 0600);

    double process_width = (b - a) / num_programs; // programs step size
    char  s_width[30];
    sprintf(s_width, "%.10lf", width);
    for (int i = 0; i < num_programs; i++) {
        double beg = a + i * process_width;
        double end = beg + process_width;

        pid_t child_pid = fork();
        if (child_pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (child_pid == 0) {
            char s_beg[30], s_end[30];
            sprintf(s_beg, "%.10lf", beg);
            sprintf(s_end, "%.10lf", end);
            execl("./calc.exe", "./calc.exe", s_beg, s_end, s_width, pipe_name, NULL);
        }
    }

    FILE *fptr;
    if((fptr = fopen(pipe_name, "r")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    double sum = 0;
    double program_sum = 0;

    for (int i = 0; i < num_programs; i++) {
        fread(&program_sum, sizeof(double), 1, fptr);
        sum += program_sum;
    }
    fclose(fptr);
    system("rm -f sums_pipe");

    time_end();

    printf("Result: %.16lf\n", sum);
    print_time();

    char top_text[50];
    sprintf(top_text, "%s %s %s", argv[0], argv[1], argv[2]);
    write_time("report.txt", top_text, "", 1);
    return 0;
}