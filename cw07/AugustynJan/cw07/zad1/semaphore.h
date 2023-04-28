#ifndef __SEM_H__
#define __SEM_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sem.h>
#include "common.h"

typedef int Semaphore;

Semaphore create_semaphore(const char *filename, int initial) {
    key_t key = ftok(getenv("HOME"), filename[0]);
    if (key == -1) {
        perror("Creating a semaphore failed on ftok");
        return -1;
    }
    Semaphore semid = semget(key, 1, 0664 | IPC_CREAT);
    if (semid == -1) {
        perror("Creating a semaphore failed on semid");
        return -1;
    }
    if(semctl(semid, 0, SETVAL, initial) == -1) {
        perror("Creating a semaphore failed on semctl");
        return -1;
    }
    return semid;
}

Semaphore open_semaphore(const char *filename) {
    key_t key = ftok(PROJECT_ID, filename[0]);
    if (key == -1) {
        return -1;
    }
    return semget(key, 1, 0);
}

void unlink_semaphore(const char* filename) {
    Semaphore semid = open_semaphore(filename);
    if( semid == -1) {
        fprintf(stderr, "[ERROR] Failed to unlink semaphore.\n");
        return;
    }
    semctl(semid, 0, IPC_RMID);
}

void acquire(Semaphore sem) {
    struct sembuf operation = { 0, -1, 0 };
    if(semop(sem, &operation, 1) == -1) {
        perror("acquire");
    }
}

void release(Semaphore sem) {
    struct sembuf operation = { 0, 1, 0 };
    if(semop(sem, &operation, 1) == -1){
        perror("acquire");
    }
}

#endif