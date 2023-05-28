#include "common.h"


int socket_fd;
void handle_SIGINT(int signum) {
    message msg = { .type = DISCONNECT };
    write(socket_fd, &msg, sizeof msg);
    exit(EXIT_SUCCESS);
}

int connect_unix_socket(char* path) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof addr.sun_path -1);

    int sock;
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0))== -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr*) &addr, sizeof addr) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    return sock;
}

int connect_web_socket(char* ipv4, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0))== -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr*) &addr, sizeof addr) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    return sock;
}

int main(int argc, char** argv) {
    if (strcmp(argv[2], "-w") == 0 && argc == 5){
        socket_fd = connect_web_socket(argv[3], atoi(argv[4]));
    } else if (strcmp(argv[2], "-u") == 0 && argc == 4){
        socket_fd = connect_unix_socket(argv[3]);
    } else {
        printf("Usage: %s [-u <path>] [-w <ipv4> <port>]\n", argv[0]);
        printf("\t-u <path>        Specify the path to a Unix socket.\n");
        printf("\t-w <ipv4> <port> Specify the IPv4 address and port number for a WebSocket connection.\n");
        exit(EXIT_SUCCESS);
    }

    signal(SIGINT, handle_SIGINT);
    char* name = argv[1];
    write(socket_fd, name, strlen(name));

    int epoll_fd;
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event stdin_event = {
            .events = EPOLLIN | EPOLLPRI,
            .data = { .fd = STDIN_FILENO }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    struct epoll_event socket_event = {
            .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
            .data = { .fd = socket_fd }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &socket_event);
    printf("Connected to server as: %s\n", name);
    puts("Commands: < STOP | LIST | 2ALL 'msg' | 2ONE 'msg' 'name' >");
    struct epoll_event events[2];
    while(1){
        int events_num = epoll_wait(epoll_fd, events, 2, 1);
        for(int i=0; i < events_num; i++){

            if (events[i].data.fd == STDIN_FILENO) {
                char buffer[BUFFER_SIZE] = {};
                int x = read(STDIN_FILENO, &buffer, BUFFER_SIZE);
                buffer[x] = 0;

                char corrector[] = " \t\n";
                char *token;
                int idx = 0;
                token = strtok( buffer, corrector );

                message_type type = -1;
                char text[MSG_LEN] = {};
                char other_name[MSG_LEN] = {};

                bool invalid = false;
                if (token == NULL) continue;

                while( token != NULL ) {
                    switch (idx) {
                        case 0:
                            if (strcmp(token, "LIST") == 0) type = LIST;
                            else if (strcmp(token, "2ALL") == 0) type = MSG_ALL;
                            else if (strcmp(token, "2ONE") == 0) type = MSG_ONE;
                            else if (strcmp(token, "STOP") == 0) type = STOP;
                            else {
                                invalid = true;
                                puts("Invalid command");
                                puts("Usage: < STOP | LIST | 2ALL 'msg' | 2ONE 'msg' 'name' >");
                                type = -1;
                            }
                            break;
                        case 1:
                            memcpy(text, token, strlen(token)*sizeof(char));
                            text[strlen(token)] = 0;
                            break;
                        case 2:
                            memcpy(other_name, token, strlen(token)*sizeof(char));
                            other_name[strlen(token)] = 0;
                            break;
                        case 3:
                            invalid = true;
                            break;
                    }
                    if (invalid) break;

                    idx++;
                    token = strtok( NULL, corrector );
                }
                if (invalid) continue;

                message msg;
                msg.type = type;
                memcpy(&msg.other_name, other_name, strlen(other_name)+1);
                memcpy(&msg.text, text, strlen(text)+1);

                write(socket_fd, &msg, sizeof msg);
            }
            else {
                message msg;
                read(socket_fd, &msg, sizeof msg);
                if (events[i].events & EPOLLHUP) {
                    puts("Disconnected\n");
                    exit(EXIT_SUCCESS);
                }
                switch (msg.type){
                    case USERNAME_TAKEN:
                        puts("This client name is already taken\n");
                        close(socket_fd);
                        exit(EXIT_SUCCESS);
                    case SERVER_FULL:
                        puts("Server is full\n");
                        close(socket_fd);
                        exit(EXIT_SUCCESS);
                    case PING:
                        write(socket_fd, &msg, sizeof msg);
                        break;
                    case STOP:
                        puts("Stopping\n");
                        close(socket_fd);
                        exit(EXIT_SUCCESS);
                    case GET:
                        puts(msg.text);
                        break;
                    default:
                }
            }
        }
    }
}