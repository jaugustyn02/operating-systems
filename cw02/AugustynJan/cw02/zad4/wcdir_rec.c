#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>

#define SUCCESS 0
#define FAILURE 1

long long total_size;


int print_file_size(const char *f_path, const struct stat *buf, int t_flag) {
    if(S_ISDIR(buf->st_mode))
        return 0;
    printf("%jd %s\n", (buf->st_size), f_path);
    total_size += buf->st_size;
    return 0;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }

    total_size = 0;
    if (ftw(argv[1], print_file_size, 20) == -1) {
        perror("ftw");
        return FAILURE;
    }

    printf("total size: %lld\n", total_size);

    return SUCCESS;
}