#include "common.h"
#include <pthread.h>

#define MAX_EVENTS 10
#define PING_INTERVAL 20
#define MSG_FILENAME "messages.txt"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epoll_fd;

void delete_client(client* client) {
    printf("Deleting client: %s\n", client->name);
    client->state = empty;
    client->name[0] = 0;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
}

void send_msg(client* client, message_type type, char text[]) {
    message msg;
    msg.type = type;
    memcpy(&msg.text, text, MSG_LEN * sizeof(char));
    write(client->fd, &msg, sizeof(msg));
}

void save_message(char* client_name, message msg);

void handle_client_message(client* client) {
    if (client->state == init) {
        int name_size = read(client->fd, client->name, sizeof client->name - 1);
        int j = client - clients;
        pthread_mutex_lock(&mutex);

        bool taken = false;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (i != j && strncmp(client->name, clients[i].name, sizeof clients->name) == 0) {
                message msg = { .type = USERNAME_TAKEN };
                printf("Client name %s already taken\n", client->name);
                write(client->fd, &msg, sizeof msg);
                strcpy(client->name, "new client");
                delete_client(client);
                taken = true;
                break;
            }
        }
        if (!taken) {
            client->name[name_size] = '\0';
            client->state = ready;
            printf("New client connected: %s\n", client->name);
        }
        pthread_mutex_unlock(&mutex);
    }
    else {
        message msg;
        read(client->fd, &msg, sizeof msg);
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
}

void init_socket(int socket, void* addr, int addr_size) {
    if (bind(socket, (struct sockaddr*) addr, addr_size) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socket, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    struct epoll_event event = { .events = EPOLLIN | EPOLLPRI };
    event_data* event_data = event.data.ptr = malloc(sizeof *event_data);
    event_data->type = socket_event;
    event_data->payload.socket = socket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

client* new_client(int client_fd) {
    pthread_mutex_lock(&mutex);
    int i = 0;
    while (i < MAX_CLIENTS && clients[i].state != empty) i++;
    if (i == MAX_CLIENTS) return NULL;

    client* client = &clients[i];

    client->fd = client_fd;
    client->state = init;
    client->responding = true;
    pthread_mutex_unlock(&mutex);
    return client;
}

void* ping(void* arg) {
    static message msg = { .type = PING };
    while (1){
        sleep(PING_INTERVAL);
        pthread_mutex_lock(&mutex);
        printf("Pinging clients...\n");
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].state != empty) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    write(clients[i].fd, &msg, sizeof msg);
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
        exit(0);
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
    int local_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    init_socket(local_sock, &local_addr, sizeof local_addr);

    int web_sock = socket(AF_INET, SOCK_STREAM, 0);
    init_socket(web_sock, &web_addr, sizeof web_addr);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    printf("Server started with port %d and path '%s'\n", port, socket_path);

    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int events_num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < events_num; i++) {
            event_data* data = events[i].data.ptr;
            if (data->type == socket_event) {
                int client_fd = accept(data->payload.socket, NULL, NULL);
                client* client = new_client(client_fd);
                if (client == NULL) {
                    printf("Server is full\n");
                    message msg = { .type = SERVER_FULL };
                    write(client_fd, &msg, sizeof msg);
                    close(client_fd);
                    continue;
                }

                event_data* event_data = malloc(sizeof(*event_data));
                event_data->type = client_event;
                event_data->payload.client = client;
                struct epoll_event event = { .events = EPOLLIN | EPOLLET | EPOLLHUP, .data = { event_data } };

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("epoll_ctl");
                    exit(EXIT_FAILURE);
                }
            } else if (data->type == client_event) {
                if (events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    delete_client(data->payload.client);
                    pthread_mutex_unlock(&mutex);
                }
                else handle_client_message(data->payload.client);
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