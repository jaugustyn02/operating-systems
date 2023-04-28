#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "common.h"
#include "shared_memory.h"
#include "semaphore.h"

static Semaphore sem_clients;
static Semaphore sem_barbers;
static Semaphore sem_buffer_mutex;


void create_semaphores(void) {
    sem_clients =  create_semaphore(SEM_CLIENTS_NAME, 0);
    sem_barbers =  create_semaphore(SEM_BARBERS_NAME, CHAIRS_NUM);
    sem_buffer_mutex =  create_semaphore(SEM_BUFFER_MUTEX_NAME, 1);
}

void close_semaphores(void) {
    close_semaphore(sem_clients);
    close_semaphore(sem_barbers);
    close_semaphore(sem_buffer_mutex);
}

void unlink_semaphores(void) {
    unlink_semaphore(SEM_CLIENTS_NAME);
    unlink_semaphore(SEM_BARBERS_NAME);
    unlink_semaphore(SEM_BUFFER_MUTEX_NAME);
}

int main(void) {
    printf("[Barbershop]: Barbers: %d, Chairs: %d, Sits in waiting room: %d, Customers: %d\n",
            BARBERS_NUM,
            CHAIRS_NUM,
            QUEUE_SIZE,
            CUSTOMERS_NUM);
    fflush(stdout);

    int *sh_mem = attach_shared_memory(PROJECT_ID, BUFFER_SIZE);
    if(sh_mem == NULL) {
        exit(EXIT_FAILURE);
    }
    initialize_shared_memory(sh_mem);

    unlink_semaphores();
    create_semaphores();

    for(int i=0; i < BARBERS_NUM; i++){
        pid_t pid = fork();
        if(pid == -1){
            fprintf(stderr, "Error: Failed to fork process.\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            execl("./barber", "./barber", NULL);
        }
    }


    for(int i=0; i < CUSTOMERS_NUM; i++){
        pid_t pid = fork();
        if(pid == -1){
            fprintf(stderr, "Error: Failed to fork process.\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            execl("./client", "./client", NULL);
        }
    }

    while(wait(NULL) > 0);

    if (!destroy_shared_memory(PROJECT_ID)) {
        fprintf(stderr, "Error: Failed to release shared memory.\n");
        exit(EXIT_FAILURE);
    }

    unlink_semaphores();
    close_semaphores();

    printf("[Barbershop] Has closed\n");
    fflush(stdout);
    return 0;
}