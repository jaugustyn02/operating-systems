#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#include<unistd.h>
#include <string.h>
#include "common.h"

int msqid;
int key;
int client_id;
int server_msqid;

pthread_t input_thread;

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
    // Send INIT to server
    msg_buf msg;
    msg.mtype = INIT;
    msg.value = key; // client msg queue key
    strcpy(msg.text, "");
    msg.sender_id = -2;
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);

    // Wait for INIT response
    msg_buf response;
    msgrcv(msqid, &response, sizeof(msg_buf) - sizeof(long), INIT, 0);
    client_id = response.value;
    if (client_id < 0){
        printf("Cannot connect: Server is fully occupied\n");
        msgctl(msqid, IPC_RMID, NULL);
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

void send_MESSAGE_ALL(){
    char temp;
    msg_buf msg;
    msg.mtype = MESSAGE_ALL;
    msg.sender_id = client_id;
    msg.value = 0;
    printf("Enter message to all clients: ");
    scanf("%c",&temp); // temp statement to clear buffer
    scanf("%1023[^\n]",msg.text);
    printf("Sending MESSAGE ALL to server\n");
    msgsnd(server_msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
}

void send_MESSAGE_ONE(){
    char temp;
    msg_buf msg;
    msg.mtype = MESSAGE_ONE;
    msg.sender_id = client_id;
    printf("Enter ID of the recipient: ");
    scanf("%d", &msg.value);
    printf("Enter message to client %d: ", msg.value);
    scanf("%c",&temp); // temp statement to clear buffer
    scanf("%1023[^\n]", msg.text);
    printf("Sending MESSAGE ONE to server\n");
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
    long msg_type;
    while(1){
        printf("Enter type of message: ");
        scanf("%ld", &msg_type);
        switch (msg_type) {
            case 2:
                msg_buf msg;
                msg.mtype = STOP;
                msg.sender_id = client_id;
                msgsnd(msqid, &msg, sizeof(msg_buf) - sizeof(long), 0);
                return NULL;
            case 3:
                send_LIST();
                break;
            case 4:
                send_MESSAGE_ALL();
                break;
            case 5:
                send_MESSAGE_ONE();
                break;
            default:
                printf("Invalid type of message\nUsage: < 2 (STOP) | 3 (LIST) | 4 (MSG ALL) | 5 (MSG ONE) >\n");
        }
    }
    return NULL;
}

int main (){
    srand(time(NULL));
    signal(SIGINT, SIGINT_handler);

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
            case 2:
                 if (msg.sender_id == -1)
                    printf("\nReceived STOP from server\n");
                 send_STOP();
            case 3:
                LIST_handler(msg);
                break;
            case 4:
                MESSAGE_ALL_handler(msg);
                break;
            case 5:
                MESSAGE_ONE_handler(msg);
                break;
        }
    }
}