#ifndef __SHARED_MEMORY_H__
#define __SHARED_MEMORY_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"


static int get_shared_memory(const char *filename, int size) {
    key_t key = ftok(filename, 0);
    if (key == -1) {
        return -1;
    }
    return shmget(key, size, 0644 | IPC_CREAT);
}

int *attach_shared_memory(const char* filename, int size) {
    int shared_memory_id = get_shared_memory(filename, size);
    if (shared_memory_id == -1) {
        fprintf(stderr, "Error: No identifier for file: %s\n", filename);
        return NULL;
    }
    int *shared_memory = shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (void*) -1) {
        fprintf(stderr, "Error: Failed to load block with id %d\n", shared_memory_id);
        return NULL;
    }
    return shared_memory;
}

bool detach_shared_memory(int *shared_memory) {
    return (shmdt(shared_memory) != -1);
}

bool destroy_shared_memory(const char *filename) {
    int shared_memory_id = get_shared_memory(filename, 0);
    if(shared_memory_id == -1) {
        return false;
    }
    return (shmctl(shared_memory_id, IPC_RMID, 0) != -1);
}

// reset functions

void resetNextHaircut(int *shared_memory) {
    shared_memory[0] = -1;
}

void resetWaitingClientsNumber(int *shared_memory) {
    shared_memory[1] = 0;
}

void resetStartingClientPid(int *shared_memory) {
    shared_memory[2] = -1;
}

void resetFinishedClientPid(int *shared_memory) {
    shared_memory[3] = -1;
}

// init

void initialize_shared_memory(int *shared_memory) {
    resetNextHaircut(shared_memory);
    resetWaitingClientsNumber(shared_memory);
    resetStartingClientPid(shared_memory);
    resetFinishedClientPid(shared_memory);
}

// getters and setters

bool setNextHaircut(int *shared_memory, int haircut) {
    if (shared_memory[0] != -1) {
        return false;
    }
    shared_memory[0] = haircut;
    return true;
}

int getNextHaircut(const int *shared_memory) {
    return shared_memory[0];
}

bool incrementWaitingClientsNumber(int *shared_memory) {
    if (shared_memory[1] == -1) {
        return false;
    }
    shared_memory[1]++;
    return true;
}

bool decrementWaitingClientsNumber(int *shared_memory) {
    if (shared_memory[1] == -1) {
        return false;
    }
    shared_memory[1]--;
    return true;
}

void setWaitingClientsNumber(int *shared_memory, int number) {
    shared_memory[1] = number;
}

int getWaitingClientsNumber(const int *shared_memory) {
    return shared_memory[1];
}

bool waitingRoomIsFull(const int *shared_memory) {
    return (shared_memory[1] == QUEUE_SIZE);
}

bool setStartingClientPid(int *shared_memory, pid_t pid) {
    if (shared_memory[2] != -1) {
        return false;
    }
    shared_memory[2] = (int)pid;
    return true;
}

pid_t getStartingClientPid(const int *shared_memory){
    return (pid_t)shared_memory[2];
}

bool setFinishedClientPid(int *shared_memory, pid_t pid) {
    if (shared_memory[3] != -1) {
        return false;
    }
    shared_memory[3] = (int)pid;
    return true;
}

pid_t getFinishedClientPid(const int *shared_memory){
    return (pid_t)shared_memory[3];
}

#endif