#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rus_time.h"

#define SUCCESS 0
#define FAILURE 1


int main(int argc, char* argv[]){
    time_start();

    // Arguments validation

    if(argc != 5){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }
    if(strlen(argv[1]) != 1){
        fprintf(stderr, "Error: Invalid first argument\n");
        return FAILURE;
    }
    if(strlen(argv[2]) != 1){
        fprintf(stderr, "Error: Invalid second argument\n");
        return FAILURE;
    }
    const char from=(char)*argv[1], to=(char)*argv[2];

    int source_fd = open(argv[3], O_RDONLY);
    if (source_fd == -1)
    {
        fprintf(stderr, "Error: Cannot open source file: %s\n", argv[3]);
        return FAILURE;
    }
    int dest_fd = open(argv[4], O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|O_TRUNC);

    // Reading source file and writing result to destination file

    char c;
    while (read(source_fd, &c, (size_t)1) == 1){
        if(c == from) c = to;
        write(dest_fd, &c, (size_t)1);
    }

    close(source_fd);
    close(dest_fd);

    time_end();
    write_time("pomiar_zad_1.txt", "tr_sys.exe", "", 1);

    return SUCCESS;
}