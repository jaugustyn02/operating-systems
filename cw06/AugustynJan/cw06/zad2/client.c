#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

mqd_t client_mq;
mqd_t server_mq;
int client_id;
char mq_name[CLIENT_MQ_NAME_LEN];

pthread_t input_thread;

void clean_up(){
    if (mq_close(server_mq) == -1) {
        perror("mq_close");
        return;
    }
    if (mq_close(client_mq) == -1) {
        perror("mq_close");
        return;
    }
    mq_unlink(mq_name);
    printf("Message queue closed successfully\n");
}

void create_queue(){
    mq_name[0] = '/';
    for(int i = 1; i < CLIENT_MQ_NAME_LEN; i++) {
        mq_name[i] = 'A' + rand() % 26;
    }
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_CLIENTS;
    attr.mq_msgsize = sizeof(msg_buf);
    if((client_mq = mq_open(mq_name, O_RDWR | O_CREAT, 0666, &attr)) == -1){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("Client message queue created\n");
}

void send_INIT(){
    printf("Sending INIT to server\n");
    msg_buf msg;
    msg.mtype = INIT;
    msg.value = 0;
    strcpy(msg.text, mq_name);
    msg.sender_id = -2;
    mq_send(server_mq, (char *) &msg, sizeof(msg_buf), 0);
    mq_receive(client_mq, (char *) &msg, sizeof(msg_buf), NULL);
    client_id = msg.value;
    if (client_id == -1){
        printf("Cannot connect: Server is fully occupied\n");
        exit(EXIT_SUCCESS);
    }
    printf("Received client ID: %d\n", client_id);
}

void send_STOP(){
    msg_buf msg;
    msg.mtype = STOP;
    msg.sender_id = client_id;
    strcpy(msg.text, "");
    msg.value = 0;
    printf("Sending STOP to server\n");
    mq_send(server_mq, (char *) &msg, sizeof(msg_buf), 0);
    exit(EXIT_SUCCESS);
}

void send_LIST(){
    msg_buf msg;
    msg.mtype = LIST;
    msg.sender_id = client_id;
    strcpy(msg.text, "");
    msg.value = 0;
    printf("sending LIST to server\n");
    mq_send(server_mq, (char *) &msg, sizeof(msg_buf), 0);
    usleep(10000);
}

void send_MESSAGE_ALL(char *text){
    msg_buf msg;
    msg.mtype = MESSAGE_ALL;
    msg.sender_id = client_id;
    msg.value = 0;
    strcpy(msg.text, text);
    mq_send(server_mq, (char *) &msg, sizeof(msg_buf), 0);
}

void send_MESSAGE_ONE(int recipient_id, char* text){
    msg_buf msg;
    msg.mtype = MESSAGE_ONE;
    msg.sender_id = client_id;
    msg.value = recipient_id;
    strcpy(msg.text, text);
    mq_send(server_mq, (char *) &msg, sizeof(msg_buf), 0);
}

void LIST_handler(msg_buf msg){
    printf("List of connected clients: %s\n", msg.text);
}

void MESSAGE_ALL_handler(msg_buf msg){
    printf("\nReceived message from client %d: %s\n", msg.sender_id, msg.text);
}

void MESSAGE_ONE_handler(msg_buf msg){
    printf("\nReceived message from client %d: %s\n", msg.sender_id, msg.text);
}

void SIGINT_handler(){
    pthread_cancel(input_thread);
    printf("\n");
    send_STOP();
}

void* input_function(void* arg) {
    printf("Usage: < STOP | LIST | 2ALL 'msg' | 2ONE ClientID 'msg' >\n");
    size_t len = 0;
    char *command = NULL;
    while (1) {
        ssize_t read;
        printf("Enter command: ");
        read = getline(&command, &len, stdin);
        command[read - 1] = '\0';

        if (strcmp(command, "") == 0) {
            continue;
        }

        char *curr_cmd = strtok(command, " ");
        if (strcmp(curr_cmd, "LIST") == 0) {
            send_LIST();
        } else if (strcmp(curr_cmd, "2ALL") == 0) {
            curr_cmd = strtok(NULL, "\0");
            char *text = curr_cmd;
            send_MESSAGE_ALL(text);
        } else if (strcmp(curr_cmd, "2ONE") == 0) {
            curr_cmd = strtok(NULL, " ");
            int recipient_id = atoi(curr_cmd);
            curr_cmd = strtok(NULL, "\0");
            char *text = curr_cmd;
            send_MESSAGE_ONE(recipient_id, text);
        } else if (strcmp(curr_cmd, "STOP") == 0) {
            msg_buf msg;
            msg.mtype = STOP;
            msg.sender_id = client_id;
            mq_send(client_mq, (char *) &msg, sizeof(msg_buf), 0);
            return NULL;
        } else {
            printf("Invalid command!\nUsage: < STOP | LIST | 2ALL 'msg' | 2ONE ClientID 'msg' >\n");
        }
    }
}

int main (){
    srand(time(NULL));
    signal(SIGINT, SIGINT_handler);
    atexit(clean_up);

    create_queue();
    server_mq = mq_open(SERVER_MQ_NAME, O_RDWR);
    if (server_mq == -1){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    send_INIT();

    pthread_create(&input_thread, NULL, &input_function, NULL);

    msg_buf msg;
    while(1) {
        mq_receive(client_mq, (char *) &msg, sizeof(msg_buf), NULL);
        switch (msg.mtype) {
            case STOP:
                if (msg.sender_id == -1)
                    printf("\nReceived STOP from server\n");
                send_STOP();
            case LIST:
                LIST_handler(msg);
                break;
            case MESSAGE_ALL:
                MESSAGE_ALL_handler(msg);
                break;
            case MESSAGE_ONE:
                MESSAGE_ONE_handler(msg);
                break;
        }
    }
}