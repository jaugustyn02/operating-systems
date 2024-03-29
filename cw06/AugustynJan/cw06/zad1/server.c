#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "common.h"

#define MAX_CLIENTS 10
#define MSG_FILENAME "messages.txt"

FILE *msg_file;

int msqid;  // servers msg qid
int clients_num = 0; // num of connected clients
int client_msqids[MAX_CLIENTS]; // array of connected client qids


void INIT_handler(msg_buf msg){
    key_t client_key = (key_t) msg.value;
    int client_msqid = msgget(client_key, 0666 | IPC_CREAT);
    msg_buf response;
    response.mtype = INIT;

    if (clients_num < MAX_CLIENTS) {
        // Find free client ID
        int new_client_id;
        for (new_client_id = 0; new_client_id < MAX_CLIENTS; new_client_id++){
            if (client_msqids[new_client_id] == -1)
                break;
        }
        if (new_client_id == MAX_CLIENTS){
            printf("Error: cannot find free client ID\n");
            return;
        }
        response.value = new_client_id;
        strcpy(response.text, "");

        // Add client
        client_msqids[new_client_id] = client_msqid;
        printf("New client connected with ID: %d\n", new_client_id);
        ++clients_num;
    }
    else{
        response.value = -1;
        strcpy(response.text, "The server has reached its maximum capacity for connected clients");
        printf("The server has reached its maximum capacity for connected clients\n");
    }
    msgsnd(client_msqid, &response, sizeof(msg_buf) - sizeof(long), 0); // Send response
}

void STOP_handler(msg_buf msg){
    int sender_id = msg.sender_id;
    printf("Received STOP from client %d\n", sender_id);
    if (sender_id < 0 || sender_id >= MAX_CLIENTS){
        printf("[STOP] - Invalid client ID: %d\n", sender_id);
        return;
    }
    if (client_msqids[sender_id] == -1){
        printf("[STOP] - Client %d is not connected\n", sender_id);
        return;
    }
    client_msqids[sender_id] = -1;
    --clients_num;
}


void LIST_handler(msg_buf msg){
    msg_buf response;
    response.mtype = LIST;
    strcpy(response.text, "");
    char id[12];
    int first_client_flag = 0;
    for (int client_id = 0; client_id < MAX_CLIENTS; client_id++){
        if(client_msqids[client_id] != -1){
            if (first_client_flag){
                strcat(response.text, ", ");
            }
            sprintf(id, "%d", client_id);
            strcat(response.text, id);
            if (first_client_flag == 0){
                first_client_flag = 1;
            }
        }
    }
    msgsnd(client_msqids[msg.sender_id], &response, sizeof(msg_buf) - sizeof(long), 0);
    printf("List of clients sent to client %d\n", msg.sender_id);
}

void MESSAGE_ALL_handler(msg_buf msg){
    for (int client_id = 0; client_id < MAX_CLIENTS; client_id++){
        if(client_msqids[client_id] != -1 && client_id != msg.sender_id){
            msgsnd(client_msqids[client_id], &msg, sizeof(msg_buf) - sizeof(long), 0);
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
    if (client_msqids[recipient] == -1){
        printf("[MESSAGE ONE] - Client with ID %d is not connected\n", recipient);
        return;
    }
    msgsnd(client_msqids[recipient], &msg, sizeof(msg_buf) - sizeof(long), 0);
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
    msgsnd(client_msqids[client_id], &msg, sizeof(msg_buf) - sizeof(long), 0);

    msg_buf response;
    do{
        msgrcv(msqid, &response, sizeof(msg_buf) - sizeof(long), STOP, 0);
    }while (response.sender_id != client_id);
    printf("Received STOP from client %d\n", response.sender_id);
    save_msg_to_file(response);
}

void SIGINT_handler(){
    printf("\n");
    for (int client_id=0; client_id < MAX_CLIENTS; client_id++)
        if(client_msqids[client_id] != -1)
            send_STOP(client_id);

    exit(EXIT_SUCCESS);
}

void clean_up(){
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    printf("Server message queue closed successfully\n");
}


int main() {
    atexit(clean_up);
    signal(SIGINT, SIGINT_handler);

    // initialize clients array
    for (int i=0; i<MAX_CLIENTS; i++)
        client_msqids[i] = -1;

    // Create server msg queue
    key_t key = ftok(getenv(KEY_PATH_ENV), SERVER_KEYGEN_NUM);
    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    printf("Server message queue created with ID: %d\n", msqid);

    msg_buf msg;
    while(1) {
        msgrcv(msqid, &msg, sizeof(msg_buf) - sizeof(long), 0, 0);
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