#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

int msqid;
int key;
int client_id;
int server_msqid;

pthread_t input_thread;

void clean_up(){
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return;
    }
    printf("Message queue closed successfully\n");
}

void create_queue(){
    char rand_num = (rand() % CLIENT_KEYGEN_NUM_RANGE) + 2;
    key = ftok(getenv(KEY_PATH_ENV), rand_num);
    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    printf("Client message queue created with ID: %d\n", msqid);
}

void send_INIT(){
    msg_buf msg;
    msg.mtype = INIT;
    msg.value = key; // client msg queue key
    strcpy(msg.text, "");
    msg.sender_id = -2;
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);

    msg_buf response;
    msgrcv(msqid, &response, sizeof(msg_buf) - sizeof(long), INIT, 0);
    client_id = response.value;
    if (client_id < 0){
        printf("Cannot connect: Server is fully occupied\n");
        exit(EXIT_SUCCESS);
    }
    printf("Received client ID: %d\n", response.value);
}

void send_STOP(){
    msg_buf msg;
    msg.mtype = STOP;
    msg.sender_id = client_id;
    strcpy(msg.text, "");
    msg.value = 0;
    printf("Sending STOP to server\n");
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
    exit(EXIT_SUCCESS);
}

void send_LIST(){
    msg_buf msg;
    msg.mtype = LIST;
    msg.sender_id = client_id;
    strcpy(msg.text, "");
    msg.value = 0;
    printf("sending LIST to server\n");
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
    usleep(10000);
}

void send_MESSAGE_ALL(char *text){
    msg_buf msg;
    msg.mtype = MESSAGE_ALL;
    msg.sender_id = client_id;
    msg.value = 0;
    strcpy(msg.text, text);
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
}

void send_MESSAGE_ONE(int recipient_id, char* text){
    msg_buf msg;
    msg.mtype = MESSAGE_ONE;
    msg.sender_id = client_id;
    msg.value = recipient_id;
    strcpy(msg.text, text);
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
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
    ssize_t read;
    char *command = NULL;
    while (1) {
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
            msgsnd(msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
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

    // Create client msg queue
    create_queue();

    // Open server msg queue
    key_t server_key = ftok(getenv(KEY_PATH_ENV), SERVER_KEYGEN_NUM);
    server_msqid = msgget(server_key, 0666 | IPC_CREAT);

    // Send INIT msg
    send_INIT();

    // create input thread
    pthread_create(&input_thread, NULL, &input_function, NULL);

    msg_buf msg;
    while(1) {
        msgrcv(msqid, &msg, sizeof(msg_buf) - sizeof(long), 0, 0);
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