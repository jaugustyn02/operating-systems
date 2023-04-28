#ifndef __COMMON_H__
#define __COMMON_H__
#include <semaphore.h>

// Semaphore
#define PROJECT_ID "SHARED"
#define SEM_CLIENTS_NAME "/clients"
#define SEM_BARBERS_NAME "/barbers"
#define SEM_BUFFER_MUTEX_NAME "/mutex"

// Simulation config
#define BARBERS_NUM 3   // M
#define CHAIRS_NUM 2    // N
#define QUEUE_SIZE 4    // P
#define CUSTOMERS_NUM 7
#define HAIRCUTS_NUM 10  // F
int haircuts_times[HAIRCUTS_NUM] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
#define BUFFER_SIZE sizeof(int) * 4 // {int next_haircut, int waiting_clients_num, int starting_client_pid, int finished_client_pid}
#define TIME_BREAK 1000

#endif
