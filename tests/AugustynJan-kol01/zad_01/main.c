#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUF_SIZE 256

/*
 * Funkcja 'get_date' powinna
 * 1) wywołać polecenie 'date'
 * 2) z jego wyjścia standardowego wczytać linijkę do bufora 'buf'
 * 3) poczekać na zakończenie utworzonego procesu i zwrócić jego kod wyjścia
 */
int get_date(char* buf, int size) {
    FILE* date_pipe = popen("date", "r");
    if (date_pipe == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    if (fgets(buf, size, date_pipe) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    int status = pclose(date_pipe);
    if (status == -1) {
        perror("pclose");
        exit(EXIT_FAILURE);
    }
    return status;
}

int main(void) {
    char buf[BUF_SIZE];
    get_date(buf, BUF_SIZE);

    if (wait(NULL) != -1) {
        printf("Child not reaped\n");
    }
    printf("Date: %s", buf);
    return 0;
}
