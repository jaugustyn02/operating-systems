#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "total_time.h"


double f(double x) {
    return 4.0 / (x * x + 1);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <rectangle width> <number of processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    double a = 0, b = 1;
    double width = atof(argv[1]);
    if (width > b - a){
        printf("Error: Rectangle width cannot be greater than integration interval\n");
        exit(EXIT_FAILURE);
    }
    int num_processes = atoi(argv[2]);
    if (num_processes < 1){
        printf("Error: Number of processes must be greater or equal 1\n");
        exit(EXIT_FAILURE);
    }

    time_start();

    int fd[num_processes][2]; // pipes for communication
    pid_t pid[num_processes]; // process IDs
    double process_width = (b - a) / num_processes; // processes step size

    for (int i = 0; i < num_processes; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid[i] == 0) {
            close(fd[i][0]); // close read end of the pipe

            double local_sum = 0;
            double beg = a + i * process_width;
            double end = beg + process_width;

            double next_x = width/2;
            while (next_x < beg) next_x += width;

            for (double xi = next_x; xi < end; xi += width) {
                local_sum += f(xi) * width;
            }

            write(fd[i][1], &local_sum, sizeof(double));
            close(fd[i][1]); // close write end of the pipe

            exit(EXIT_SUCCESS);
        } else {
            close(fd[i][1]); // close write end of the pipe
        }
    }

    double sum = 0;
    for (int i = 0; i < num_processes; i++) {
        double process_sum;
        read(fd[i][0], &process_sum, sizeof(double));
        sum += process_sum;
        close(fd[i][0]); // close read end of the pipe
    }
    time_end();

    printf("Result: %.16lf\n", sum);
    print_time();

    char top_text[50];
    sprintf(top_text, "%s %s %s", argv[0], argv[1], argv[2]);
    write_time("report.txt", top_text, "", 1);
    return 0;
}
