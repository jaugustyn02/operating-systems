#include <stdio.h>
#include <unistd.h>

#define SUCCESS 0
#define FAILURE 1


int main(int argc, char* argv[]){

    // Arguments validation
    if(argc != 2){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }

    printf("%s ", __FILE__);
    fflush(stdout);

    execl("/bin/ls", "ls", argv[1], NULL);

    perror("Cannot run program /bin/ls\n");
    return FAILURE;
}