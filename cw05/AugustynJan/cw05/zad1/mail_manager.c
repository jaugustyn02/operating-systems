#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024


void send_email(char *to, char *subject, char *body) {
    char command[MAX_LEN];
    snprintf(command, MAX_LEN, "echo \"%s\" | mailx -s \"%s\" \"%s\"", body, subject, to);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute command: %s\n", command);
        exit(1);
    }

    char response[MAX_LEN];
    while (fgets(response, MAX_LEN, fp) != NULL) {
        printf("%s", response);
    }

    pclose(fp);
}

void list_emails(char *sort_by) {
    char command[MAX_LEN];
    if (strcmp(sort_by, "nadawca") == 0) {
        snprintf(command, MAX_LEN, "mail -H | sort -k 2 | awk '/^ *[0-9]/ {print $2}' | uniq");
    } else if (strcmp(sort_by, "data") == 0) {
        snprintf(command, MAX_LEN, "mail -H | sort -k 3 | awk '/^ *[0-9]/ {print $3}' | uniq");
    } else {
        printf("Invalid argument: %s\n", sort_by);
        exit(1);
    }

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute command: %s\n", command);
        exit(1);
    }

    char email[MAX_LEN];
    while (fgets(email, MAX_LEN, fp) != NULL) {
        printf("%s", email);
    }

    pclose(fp);
}


int main(int argc, char *argv[]) {
    if (argc == 2) {
        list_emails(argv[1]);
    } else if (argc == 4) {
        char *to = argv[1];
        char *subject = argv[2];
        char *body = argv[3];
        send_email(to, subject, body);
    } else {
        printf("Usage: %s <sort_by> OR %s <to> <subject> <body>\n", argv[0], argv[0]);
        return 1;
    }

    return 0;
}