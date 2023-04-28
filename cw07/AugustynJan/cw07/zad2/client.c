#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "shared_memory.h"
#include "common.h"
#include "semaphore.h"

static Semaphore sem_clients;
static Semaphore sem_barbers;
static Semaphore sem_buffer_mutex;

void open_semaphores(){
    sem_clients = open_semaphore(SEM_CLIENTS_NAME);
    sem_barbers = open_semaphore(SEM_BARBERS_NAME);
    sem_buffer_mutex = open_semaphore(SEM_BUFFER_MUTEX_NAME);
}

int random_haircut() {
    return (int)(rand() % HAIRCUTS_NUM);
}

int main(void) {
    srand(time(NULL) * getpid());

    int *sh_mem = attach_shared_memory(PROJECT_ID, BUFFER_SIZE);
    if (sh_mem == NULL) {
        fprintf(stderr, "Error: Can't open shared memory\n");
        exit(EXIT_FAILURE);
    }

    open_semaphores();

    acquire(sem_buffer_mutex);
    if (waitingRoomIsFull(sh_mem)){
        printf("[Client:%d] Is leaving because waiting room is full\n", (int)getpid());
        fflush(stdout);

        release(sem_buffer_mutex);
        detach_shared_memory(sh_mem);
        exit(EXIT_SUCCESS);
    }
    incrementWaitingClientsNumber(sh_mem);

    printf("[Client:%d] Sits down in waiting room\n", (int)getpid());
    fflush(stdout);
    release(sem_buffer_mutex);

    // waiting in queue for his turn (free barber with chair)
    acquire(sem_barbers);

    // client no longer sits in waiting room
    acquire(sem_buffer_mutex);
    decrementWaitingClientsNumber(sh_mem);
    release(sem_buffer_mutex);

    // inform barber about haircut
    int haircut = random_haircut();
    while(true) {
        acquire(sem_buffer_mutex);
        bool status = setNextHaircut(sh_mem, haircut);
        if(status){
            setStartingClientPid(sh_mem, getpid());
            release(sem_buffer_mutex);
            break;
        }
        release(sem_buffer_mutex);
        usleep(TIME_BREAK);
    }

    printf("[Client:%d] Sits down in char and asks for haircut %d\n", (int)getpid(), (int)haircut);
    fflush(stdout);

    // wake up barber because new client is ready
    release(sem_clients);

    // wait for haircut to be done
    while(true) {
        acquire(sem_buffer_mutex);
        if(getFinishedClientPid(sh_mem) == getpid()) {
            resetFinishedClientPid(sh_mem);
            release(sem_buffer_mutex);
            break;
        }
        release(sem_buffer_mutex);
        usleep(TIME_BREAK);
    }

    printf("[Client:%d] Is leaving with new haircut\n", (int)getpid());
    fflush(stdout);

    // barber with chair is free
    release(sem_barbers);

    detach_shared_memory(sh_mem);

    return 0;
}