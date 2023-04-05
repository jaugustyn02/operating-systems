#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


#define SUCCESS 0
#define FAILURE 1
#define BUFFER_SIZE 255

int process_file(char* path, char * search_str){
    int fd;
    if ((fd = open(path, O_RDONLY)) == -1) {
        perror("Open");
        return FAILURE;
    }

    char buffer[BUFFER_SIZE];
    if (read(fd, buffer, strlen(search_str)) == -1){
        perror("Read");
        return FAILURE;
    }

    if (strncmp(buffer, search_str, strlen(search_str)) == 0){
        printf("File path: %s\tPID: %d\n", path, (int)getpid());
    }

    if(close(fd) == -1){
        perror("Close");
        return FAILURE;
    }
    return SUCCESS;
}

void read_directory(char* path, char* search_str){
    DIR* DP;
    if((DP = opendir(path)) == NULL){
        perror("Opendir");
        exit(FAILURE);
    }

    struct dirent* dp;
    while((dp = readdir(DP)) != NULL){
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
            continue;
        }

        char new_path[PATH_MAX];
        snprintf(new_path, sizeof(new_path), "%s/%s", path, dp->d_name);    // set new path (path/dp->name)

        struct stat stat_buf;
        if(stat(new_path, &stat_buf) == -1){
            perror("Stat");
            continue;
        }
        if(S_ISDIR(stat_buf.st_mode)) {
            // file is a directory
            pid_t pid = fork();
            if (pid == -1) {
                perror("Fork");
                exit(FAILURE);
            }
            if (pid == 0) {
                read_directory(new_path, search_str);
                while (wait(NULL) > 0);
                exit(SUCCESS);
            }
        }
        else if (S_ISREG(stat_buf.st_mode)){
            // file is a regular file
            process_file(new_path, search_str);
        }
    }
    if (errno != 0){
        perror("Errno");
        exit(FAILURE);
    }

    if(closedir(DP) == -1){
        perror("Closedir");
        exit(FAILURE);
    }

}

int main(int argc, char* argv[]){

    // Arguments validation
    if(argc != 3){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }

    char* dir_path = argv[1];
    char* search_str = argv[2];
    if(strlen(search_str) > BUFFER_SIZE){
        fprintf(stderr, "Error: Second argument is longer than: %d\n", BUFFER_SIZE);
        return FAILURE;
    }

    read_directory(dir_path, search_str);

    while(wait(NULL) > 0);
    fflush(stdout);

    return SUCCESS;
}