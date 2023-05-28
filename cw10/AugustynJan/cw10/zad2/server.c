#include "common.h"
#include <pthread.h>

#define MAX_EVENTS 10
#define PING_INTERVAL 20
#define MSG_FILENAME "messages.txt"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epoll_fd;

void delete_client(client* client);

void handle_SIGINT(int signum) {
    message msg = { .type = DISCONNECT };
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].state != empty) {
            printf("Sending DISCONNECT to %s\n", clients[i].name);
//            delete_client(&clients[i]);
            sendto(clients[i].socket, &msg, sizeof msg, 0, (sa) &clients[i].addr, clients[i].addr_len);
        }
    }
    exit(EXIT_SUCCESS);
}

void delete_client(client* client) {
    printf("Deleting client: %s\n", client->name);
    message msg = { .type = STOP };
    sendto(client->socket, &msg, sizeof msg, 0, (sa) &client->addr, client->addr_len);
    memset(&client->addr, 0, sizeof client->addr);
    client->state = empty;
    client->socket = 0;
    client->name[0] = 0;
}

void send_msg(client* client, message_type type, char text[]) {
    message msg;
    msg.type = type;
    memcpy(&msg.text, text, MSG_LEN * sizeof(char));
    sendto(client->socket, &msg, sizeof msg, 0, (sa) &client->addr, client->addr_len);
}

void save_message(char* client_name, message msg);

void handle_client_message(client* client, const message* msg_ptr) {
    message msg = *msg_ptr;
    save_message(client->name, msg);

    if (msg.type == PING) {
        pthread_mutex_lock(&mutex);
        printf("PONG received from %s\n", client->name);
        client->responding = true;
        pthread_mutex_unlock(&mutex);
    } else if (msg.type == DISCONNECT || msg.type == STOP) {
        pthread_mutex_lock(&mutex);
        delete_client(client);
        pthread_mutex_unlock(&mutex);
    } else if (msg.type == MSG_ALL) {
        char out[BUFFER_SIZE] = "";
        strcat(out, client->name);
        strcat(out, ": ");
        strcat(out, msg.text);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != empty)
                send_msg(clients+i, GET, out);
        }
    } else if (msg.type == LIST) {
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != empty)
                send_msg(client, GET, clients[i].name);
        }
    } else if (msg.type == MSG_ONE) {
        char out[BUFFER_SIZE] = "";
        strcat(out, client->name);
        strcat(out, ": ");
        strcat(out, msg.text);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != empty) {
                if (strcmp(clients[i].name, msg.other_name) == 0) {
                    send_msg(clients+i, GET, out);
                }
            }
        }
    }
}

void init_socket(int socket, void* addr, int addr_size) {
    if (bind(socket, (struct sockaddr*) addr, addr_size) == -1) {
        perror("bind");
        exit(1);
    }
    struct epoll_event event = {
            .events = EPOLLIN | EPOLLPRI,
            .data = { .fd = socket }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

void new_client(union addr* addr, socklen_t addr_len, int socket, char* name) {
    pthread_mutex_lock(&mutex);
    int empty_index = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state == empty) empty_index = i;
        else if (strncmp(name, clients[i].name, sizeof clients->name) == 0) {
            pthread_mutex_unlock(&mutex);
            message msg = {.type = USERNAME_TAKEN };
            printf("Client name %s already taken\n", name);
            sendto(socket, &msg, sizeof msg, 0, (sa) addr, addr_len);
            return;
        }
    }
    if (empty_index == -1) {
        pthread_mutex_unlock(&mutex);
        printf("Server is full\n");
        message msg = { .type = SERVER_FULL };
        sendto(socket, &msg, sizeof msg, 0, (sa) addr, addr_len);
        return;
    }
    printf("New client connected: %s\n", name);
    client* client = &clients[empty_index];
    memcpy(&client->addr, addr, addr_len);
    client->addr_len = addr_len;
    client->state = ready;
    client->responding = true;
    client->socket = socket;

    memset(client->name, 0, sizeof client->name);
    strncpy(client->name, name, sizeof client->name - 1);

    pthread_mutex_unlock(&mutex);
}

void* ping(void* arg) {
    const static message msg = { .type = PING };
    while (1){
        sleep(PING_INTERVAL);
        pthread_mutex_lock(&mutex);
        printf("Pinging clients...\n");
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != empty) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    sendto(clients[i].socket, &msg, sizeof msg, 0, (sa) &clients[i].addr, clients[i].addr_len);
                }
                else delete_client(&clients[i]);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <path> <port>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    char* socket_path = argv[1];
    unsigned short port = atoi(argv[2]);

    epoll_fd = epoll_create1(0);

    struct sockaddr_un local_addr = { .sun_family = AF_UNIX };
    strncpy(local_addr.sun_path, socket_path, sizeof local_addr.sun_path);

    struct sockaddr_in web_addr = {
            .sin_family = AF_INET, .sin_port = htons(port),
            .sin_addr = { .s_addr = htonl(INADDR_ANY) },
    };

    unlink(socket_path);
    int local_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (local_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    init_socket(local_sock, &local_addr, sizeof local_addr);

    int web_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (web_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    init_socket(web_sock, &web_addr, sizeof web_addr);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    signal(SIGINT, handle_SIGINT);

    printf("Server started with port %d and path '%s'\n", port, socket_path);

    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int events_num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < events_num; i++) {
            int socket = events[i].data.fd;
            message msg;
            union addr addr;
            socklen_t addr_len = sizeof addr;
            recvfrom(socket, &msg, sizeof msg, 0, (sa) &addr, &addr_len);
            if (msg.type == INIT) {
                new_client(&addr, addr_len, socket, msg.name);
            } else {
                for (int j=0; j < MAX_CLIENTS; j++) {
                    if (clients[j].state != empty) {
                        if (memcmp(&clients[j].addr, &addr, addr_len) == 0) {
                            handle_client_message(&clients[j], &msg);
                            break;
                        }
                    }
                }
            }
        }
    }
}


void save_message(char* client_name, message msg){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%d-%d-%d %d:%d:%d %s %d %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec, client_name, msg.type, msg.text);

    FILE* file = fopen("messages.txt", "a");
    fprintf(file, "%d-%d-%d %d:%d:%d %s %d %s\n\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec, client_name, msg.type, msg.text);
    fclose(file);
}