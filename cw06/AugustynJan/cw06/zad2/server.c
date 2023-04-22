#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "common.h"

#define MSG_FILENAME "messages.txt"

FILE *msg_file;

mqd_t server_mq;
int clients_num = 0; // num of connected clients
char *client_mq_names[MAX_CLIENTS]; // array of connected client mq names


void INIT_handler(msg_buf msg){
    printf("Received INIT from client %s\n", msg.text);
    if (clients_num < MAX_CLIENTS) {
        // Find free client ID
        int new_client_id;
        for (new_client_id = 0; new_client_id < MAX_CLIENTS; new_client_id++){
            if (client_mq_names[new_client_id] == NULL)
                break;
        }
        if (new_client_id == MAX_CLIENTS){
            printf("Error: cannot find free client ID\n");
            return;
        }
        msg.value = new_client_id;

        // Add client
        client_mq_names[new_client_id] = (char *) calloc(CLIENT_MQ_NAME_LEN, sizeof(char));
        strcpy(client_mq_names[new_client_id], msg.text);
        printf("New client connected with ID: %d\n", new_client_id);
        ++clients_num;
    }
    else{
        msg.value = -1;
        printf("The server has reached its maximum capacity for connected clients\n");
    }
    mqd_t client_mq = mq_open(msg.text, O_RDWR);
    mq_send(client_mq, (char *) &msg, sizeof(msg_buf), 0);
    mq_close(client_mq);
}

void STOP_handler(msg_buf msg){
    int sender_id = msg.sender_id;
    printf("Received STOP from client %d\n", sender_id);
    if (sender_id < 0 || sender_id >= MAX_CLIENTS){
        printf("[STOP] - Invalid client ID: %d\n", sender_id);
        return;
    }
    if (client_mq_names[sender_id] == NULL){
        printf("[STOP] - Client %d is not connected\n", sender_id);
        return;
    }
    free(client_mq_names[sender_id]);
    client_mq_names[sender_id] = NULL;
    --clients_num;
}


void LIST_handler(msg_buf msg){
    msg.mtype = LIST;
    strcpy(msg.text, "");
    char id[12];
    int first_client_flag = 0;
    for (int client_id = 0; client_id < MAX_CLIENTS; client_id++){
        if(client_mq_names[client_id] != NULL){
            if (first_client_flag){
                strcat(msg.text, ", ");
            }
            sprintf(id, "%d", client_id);
            strcat(msg.text, id);
            if (first_client_flag == 0){
                first_client_flag = 1;
            }
        }
    }
    mqd_t client_mq = mq_open(client_mq_names[msg.sender_id], O_WRONLY);
    mq_send(client_mq, (char *) &msg, sizeof(msg_buf), 0);
    mq_close(client_mq);
    printf("List of clients sent to client %d\n", msg.sender_id);
}

void MESSAGE_ALL_handler(msg_buf msg){
    for (int client_id = 0; client_id < MAX_CLIENTS; client_id++){
        if(client_mq_names[client_id] != NULL && client_id != msg.sender_id){
            mqd_t client_mq = mq_open(client_mq_names[client_id], O_WRONLY);
            mq_send(client_mq, (char *) &msg, sizeof(msg_buf), 0);
            mq_close(client_mq);
            printf("Message from client %d sent to client %d\n", msg.sender_id, client_id);
        }
    }
}

void MESSAGE_ONE_handler(msg_buf msg){
    int recipient = msg.value;
    if(recipient < 0 || recipient >= MAX_CLIENTS) {
        printf("[MESSAGE ONE] - Invalid client ID: %d\n", recipient);
        return;
    }
    if (client_mq_names[recipient] == NULL){
        printf("[MESSAGE ONE] - Client with ID %d is not connected\n", recipient);
        return;
    }
    mqd_t client_mq = mq_open(client_mq_names[recipient], O_WRONLY);
    mq_send(client_mq, (char *) &msg, sizeof(msg_buf), 0);
    mq_close(client_mq);
    printf("Message from client %d sent to client %d\n", msg.sender_id, recipient);
}

void save_msg_to_file(msg_buf msg){
    msg_file = fopen(MSG_FILENAME, "a");
    if(!msg_file){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char buff[2048] = "";
    char str[21];
    time_t t;
    time(&t);
    strcat(buff, "TIME: ");
    strcat(buff, ctime(&t));
    strcat(buff, "SENDER ID: ");
    sprintf(str, "%d", msg.sender_id);
    strcat(buff, str);
    strcat(buff, "\nMTYPE: ");
    sprintf(str, "%ld", msg.mtype);
    strcat(buff, str);
    strcat(buff, "\nVALUE: ");
    sprintf(str, "%d", msg.value);
    strcat(buff, str);
    strcat(buff, "\nTEXT: ");
    strcat(buff, msg.text);
    strcat(buff, "\n\n");
    fwrite(buff, sizeof(char), strlen(buff), msg_file);

    fclose(msg_file);
}

void send_STOP(int client_id){
    printf("Sending STOP to client %d\n", client_id);
    msg_buf msg;
    msg.mtype = STOP;
    msg.sender_id = -1;
    strcpy(msg.text, "");

    mqd_t client_mq = mq_open(client_mq_names[client_id], O_WRONLY);
    mq_send(client_mq, (char *) &msg, sizeof(msg_buf), 0);
    mq_receive(server_mq, (char *) &msg, sizeof(msg_buf), NULL);
    mq_close(client_mq);

    printf("Received STOP from client %d\n", msg.sender_id);
    save_msg_to_file(msg);
}

void SIGINT_handler(){
    printf("\n");
    for (int client_id=0; client_id < MAX_CLIENTS; client_id++)
        if(client_mq_names[client_id] != NULL)
            send_STOP(client_id);

    exit(EXIT_SUCCESS);
}

void clean_up(){
    if (mq_close(server_mq) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    mq_unlink(SERVER_MQ_NAME);
    printf("Server message queue closed successfully\n");
}


int main() {
    for (int i=0; i<MAX_CLIENTS; i++)
        client_mq_names[i] = NULL;

    mq_unlink(SERVER_MQ_NAME);
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_CLIENTS;
    attr.mq_msgsize = sizeof(msg_buf);
    if ((server_mq = mq_open(SERVER_MQ_NAME, O_RDWR | O_CREAT | O_EXCL, 0666, &attr)) == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("Server message queue created\n");

    atexit(clean_up);
    signal(SIGINT, SIGINT_handler);

    msg_buf msg;
    while(1) {
        mq_receive(server_mq, (char *) &msg, sizeof(msg_buf), NULL);
        switch (msg.mtype) {
            case INIT:
                INIT_handler(msg);
                break;
            case STOP:
                STOP_handler(msg);
                break;
            case LIST:
                LIST_handler(msg);
                break;
            case MESSAGE_ALL:
                MESSAGE_ALL_handler(msg);
                break;
            case MESSAGE_ONE:
                MESSAGE_ONE_handler(msg);
                break;
            default:
                printf("Invalid message type: %ld\n", msg.mtype);
                continue;
        }
        save_msg_to_file(msg);
    }
}