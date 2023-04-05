#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int is_child = 0;

void sigusr1_handler(int sig_no){
    printf("Received signal SIGUSR1 (%d)\n", sig_no);
}

void test_ignore(){
    if (is_child == 0)
        signal(SIGUSR1, SIG_IGN);       // ignore signal SIGUSR1

    printf("PID: %d - raising SIGUSR1\n", (int) getpid());
    raise(SIGUSR1);
}

void test_mask(){
    if (is_child == 0) {
        struct sigaction act;
        sigemptyset(&act.sa_mask);                      // init empty set of signals
        sigaddset(&act.sa_mask, SIGUSR1);               // adding SIGUSR1 to set
        sigprocmask(SIG_BLOCK, &act.sa_mask, NULL);     // blocking all signals in set
    }

    printf("PID: %d - raising SIGUSR1\n", (int)getpid());
    raise(SIGUSR1);
}

void test_pending(){
    sigset_t set;

    if (is_child == 0) {
        // mask
        sigemptyset(&set);
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &set, NULL);

        printf("PID: %d - raising SIGUSR1\n", (int) getpid());
        raise(SIGUSR1);
    }

    // pending
    sigpending(&set);
    if (sigismember(&set, SIGUSR1)) {
        printf("PID: %d - signal SIGUSR1 is pending\n", (int)getpid());
    }
    else {
        printf("PID: %d - Signal SIGUSR1 is not pending\n", (int)getpid());
    }
}

int main(int argc, char *argv[]){
    if (argc != 2 && argc != 3) {
        printf("Usage: %s <ignore|mask|pending> (optional) <0|1>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(argc == 2) is_child = 0;
    else is_child = atoi(argv[2]);

    if (strcmp(argv[1], "ignore") == 0) {
        printf("Testing ignore:\n");
        test_ignore();
    } else if (strcmp(argv[1], "mask") == 0) {
        printf("Testing mask:\n");
        test_mask();
    } else if (strcmp(argv[1], "pending") == 0) {
        printf("Testing pending:\n");
        test_pending();
    } else {
        printf("Error: Invalid argument: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    fflush(NULL);
    if (is_child == 0){
        execl(argv[0], argv[0], argv[1], "1", NULL);
    }
    return 0;
}
