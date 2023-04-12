#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


double f(double x) {
    return 4.0 / (x * x + 1);
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("%d\n", argc);
        printf("Usage: %s <beg> <end> <width> <pipe_name>\n", argv[0]);
        return EXIT_FAILURE;
    }
    double width = atof(argv[3]);
    if (width <= 0){
        printf("Error: Rectangle width must be greater than 0\n");
        return EXIT_FAILURE;
    }
    double beg = atof(argv[1]);
    double end = atof(argv[2]);
    const char *pipe_name = argv[4];

    mkfifo(pipe_name, 0060);

    FILE *fptr;
    if((fptr = fopen(pipe_name, "w")) == NULL){
        perror("fopen");
        return EXIT_FAILURE;
    }

    double local_sum = 0;

    double next_x = width/2;
    while (next_x < beg) next_x += width;

    for (double xi = next_x; xi < end; xi += width) {
        local_sum += f(xi) * width;
    }

    fwrite(&local_sum, sizeof(double), 1, fptr);
    usleep(10000);
    fclose(fptr);

    return 0;
}
