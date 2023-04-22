#ifndef CW06_COMMON_H
#define CW06_COMMON_H
#include <sys/msg.h>
#include <mqueue.h>

#define MSG_TEXT_SIZE 1024

// message types
#define STOP 1
#define LIST 2
#define INIT 3
#define MESSAGE_ALL 4
#define MESSAGE_ONE 5

// queue config
#define SERVER_MQ_NAME "/SERVER"
#define CLIENT_MQ_NAME_LEN 4
#define MAX_CLIENTS 10

typedef struct {
    long mtype;       // typ komunikatu
    int value;        // klucz kolejki klienta
    int sender_id;
    char text[MSG_TEXT_SIZE];
} msg_buf;

#endif //CW06_COMMON_H
