#ifndef CW06_COMMON_H
#define CW06_COMMON_H
#include <sys/msg.h>
#include <sys/ipc.h>

#define MSG_TEXT_SIZE 1024

// message types
#define INIT 1
#define STOP 2
#define LIST 3
#define MESSAGE_ALL 4
#define MESSAGE_ONE 5

// queue config
#define KEY_PATH_ENV "HOME"
#define SERVER_KEYGEN_NUM 1
#define CLIENT_KEYGEN_NUM_RANGE 200

typedef struct {
    long mtype;       // typ komunikatu
    int value;        // klucz kolejki klienta
    int sender_id;
    char text[MSG_TEXT_SIZE];
} msg_buf;

#endif //CW06_COMMON_H
