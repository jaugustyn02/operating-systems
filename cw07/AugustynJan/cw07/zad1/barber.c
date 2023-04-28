#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "shared_memory.h"
#include "semaphore.h"

static Semaphore sem_clients;
static Semaphore sem_buffer_mutex;


void open_semaphores(){
    sem_clients = open_semaphore(SEM_CLIENTS_NAME);
    sem_buffer_mutex = open_semaphore(SEM_BUFFER_MUTEX_NAME);
}

int main(void){
    int *sh_mem = attach_shared_memory(PROJECT_ID, BUFFER_SIZE);
    if (sh_mem == NULL){
        fprintf(stderr, "Error: Can't open shared memory\n");
        exit(EXIT_FAILURE);
    }
    open_semaphores();

    while (true){
        printf("[Barber:%d] Falls asleep\n", (int)getpid());
        fflush(stdout);

        // Wait for client
        acquire(sem_clients);

        // Get next haircut and client pid
        int haircut;
        pid_t client_pid;
        while (true){
            acquire(sem_buffer_mutex);
            haircut = getNextHaircut(sh_mem);
            client_pid = getStartingClientPid(sh_mem);
            if (haircut != -1 && client_pid != -1){
                resetNextHaircut(sh_mem);
                resetStartingClientPid(sh_mem);
                release(sem_buffer_mutex);
                break;
            }
            release(sem_buffer_mutex);
            usleep(TIME_BREAK);
        }

        printf("[Barber:%d] -> [Client:%d] Wakes up and starts doing haircut %d\n", getpid(), client_pid, haircut);
        fflush(stdout);

        sleep(haircuts_times[haircut]);

        printf("[Barber:%d] -> [Client:%d] Done with haircut %d\n", getpid(), client_pid, haircut);
        fflush(stdout);

        // Inform client that his haircut is done
        while (true){
            acquire(sem_buffer_mutex);
            if (getFinishedClientPid(sh_mem) == -1){
                setFinishedClientPid(sh_mem, client_pid);
                release(sem_buffer_mutex);
                break;
            }
            release(sem_buffer_mutex);
            usleep(TIME_BREAK);
        }

        // Check if there are any clients left
        acquire(sem_buffer_mutex);
        if (getWaitingClientsNumber(sh_mem) > 0){
            release(sem_buffer_mutex);
            usleep(TIME_BREAK * 10);
            acquire(sem_buffer_mutex);
        }

        if (getWaitingClientsNumber(sh_mem) == 0){
            release(sem_buffer_mutex);
            break;
        }
        release(sem_buffer_mutex);
    }
    printf("[Barber:%d] Is leaving because waiting room is empty\n", getpid());
    fflush(stdout);

    detach_shared_memory(sh_mem);
    return 0;
}