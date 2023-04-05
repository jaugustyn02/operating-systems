#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int mode = 0;
int num_requests = 0;

void print_numbers() {
    for (int i = 1; i <= 100; i++) {
        printf("%d ", i);
    }
    printf("\n");
}

void print_time() {
    time_t t = time(NULL);
    printf("%s", ctime(&t));
}

void sig_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGUSR1) {
        pid_t sender_pid = info->si_pid;
        mode = info->si_value.sival_int;

        printf("Catcher received SIGUSR1 with mode: %d\n", mode);

        union sigval val;
        val.sival_int = mode;
        sigqueue(sender_pid, SIGUSR1, val);

        printf("Sent confirmation to sender\n");

        switch (mode) {
            case 1:
                num_requests++;
                print_numbers();
                break;
            case 2:
                num_requests++;
                print_time();
                break;
            case 3:
                num_requests++;
                printf("Number of requests: %d\n", num_requests);
                break;
            case 4:
                num_requests++;
                break;
            case 5:
                printf("Catcher exiting\n");
                return;
            default:
                printf("Invalid mode: %d\n", mode);
                break;
        }
    }
}

int main() {
    printf("Catcher PID: %d\n", (int)getpid());

    struct sigaction act;
    act.sa_sigaction = sig_handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);

    while (mode != 5) {
        if (mode != 4){
            sigsuspend(&act.sa_mask);
        }
        else{
            print_time();
            sleep(1);
        }
    }

    return 0;
}

