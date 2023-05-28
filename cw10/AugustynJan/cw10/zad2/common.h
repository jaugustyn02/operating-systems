#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

// message utils
#define MSG_LEN 256
#define NAME_LEN 16
#define BUFFER_SIZE 256

typedef enum {
    PING,
    STOP,
    LIST,
    INIT,
    USERNAME_TAKEN,
    SERVER_FULL,
    DISCONNECT,
    GET,
    MSG_ONE,
    MSG_ALL
} message_type;

typedef struct {
    message_type type;
    char text[MSG_LEN];
    char name[NAME_LEN];
    char other_name[MSG_LEN];
} message;

// server utils
#define MAX_CLIENTS 16

union addr {
    struct sockaddr_un uni;
    struct sockaddr_in web;
};
typedef struct sockaddr* sa;

struct client {
    union addr addr;
    int socket, addr_len;
    enum client_state {
        empty = 0,
        init,
        ready
    } state;
    struct client* peer;
    char name[NAME_LEN];
    bool responding;
} clients[MAX_CLIENTS];
typedef struct client client;

typedef struct event_data {
    enum event_type {
        socket_event,
        client_event
    } type;
    union payload {
        client* client;
        int socket;
    } payload;
} event_data;