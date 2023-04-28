#ifndef __SEM_H__
#define __SEM_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>

typedef sem_t* Semaphore;

Semaphore create_semaphore(const char *filename, int initial) {
    Semaphore s = sem_open(filename, O_CREAT | O_EXCL, 0644, initial);
    if (s == SEM_FAILED) {
        fprintf(stderr, "Error: Failed at creating semaphore %s\n", filename);
        return NULL;
    }
    return s;
}

Semaphore open_semaphore(const char *filename) {
    Semaphore s = sem_open(filename, 0);
    if (s == SEM_FAILED) {
        fprintf(stderr, "Error: Failed at opening semaphore %s\n", filename);
        return NULL;
    }
    return s;
}

void close_semaphore(Semaphore sem) {
    sem_close(sem);
}

void unlink_semaphore(const char* filename) {
    sem_unlink(filename);
}

void acquire(Semaphore sem) {
    sem_wait(sem);
}

void release(Semaphore sem) {
    sem_post(sem);
}

#endif