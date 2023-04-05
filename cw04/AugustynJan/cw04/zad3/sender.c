#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

//int received_confirmation = 0;

void sig_handler(int signum, siginfo_t *info, void *context){
//    received_confirmation = 1;
    printf("Received confirmation\n");
}

void send_signal(pid_t pid, int mode){
    union sigval val;
    val.sival_int = mode;
    sigqueue(pid, SIGUSR1, val);
    printf("Sending SIGUSR1 with mode %d\n", mode);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: sender <catcher_pid> <mode1> [<mode2> ...]\n");
        return 1;
    }

    pid_t catcher_pid = atoi(argv[1]);

    struct sigaction act;
    act.sa_sigaction = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    sigset_t set;
    for (int i = 2; i < argc; i++) {
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);

        int mode = atoi(argv[i]);
        send_signal(catcher_pid, mode);

        printf("Waiting for confirmation from catcher...\n");
        sigsuspend(&act.sa_mask);
    }

    return 0;
}
