#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define N_ELVES 10
#define N_REINDEER 9
#define N_PRESENTS 3 // number of presents to deliver

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_t elf_threads[N_ELVES];
static pthread_t reindeer_threads[N_REINDEER];

static sem_t sem_elves;
static sem_t sem_reindeer;

int elves_waiting = 0;
int reindeer_waiting = 0;
int waiting_elves_ids[3];

static int random_int(int min, int max){
    return min + rand() % (max - min + 1);
}

void *elf(void *arg){
    int id = (int)(intptr_t)(arg);
    srand(time(NULL) + id);
    while(1){
        sleep(random_int(2, 5)); // elf is working
        pthread_mutex_lock(&mutex);
        if(elves_waiting < 3){
            waiting_elves_ids[elves_waiting] = id;
            elves_waiting++;
            printf("Elf: czeka %d elfów na Mikołaja, %d\n", elves_waiting, id);
            if(elves_waiting == 3) {
                printf("Elf: wybudzam Mikołaja, %d\n", id);
                pthread_cond_signal(&cond);
            }
            pthread_mutex_unlock(&mutex);
            sem_wait(&sem_elves);
        } else{
            printf("Elf: samodzielnie rozwiązuję swój problem, %d\n", id);
            pthread_mutex_unlock(&mutex);
        }
    }
}

void *reindeer(void *arg){
    int id = (int)(intptr_t)(arg);
    srand(time(NULL) + id);
    while(1){
        sleep(random_int(5, 10)); // reindeer is on vacation
        pthread_mutex_lock(&mutex);
        ++reindeer_waiting;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeer_waiting, id);
        if(reindeer_waiting == 9){
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&sem_reindeer);
    }
}

void *santa(void *arg){
    srand(time(NULL));
    int presents_delivered = 0;
    while(presents_delivered < N_PRESENTS){
        pthread_mutex_lock(&mutex);
        while (1) {
            pthread_cond_wait(&cond, &mutex);
            if (elves_waiting == 3 || reindeer_waiting == 9) {
                printf("Mikołaj: budzę się\n");
                break;
            } else {
                perror("Mikołaj: zostaje błędnie wybudzony\n");
            }
        }
        if (elves_waiting == 3){
            printf("Mikołaj: rozwiązuję problemy elfów %d %d %d\n",
                   waiting_elves_ids[0],
                   waiting_elves_ids[1],
                   waiting_elves_ids[2]
                  );
            sleep(random_int(1, 2));
            elves_waiting = 0;
            for(int i = 0; i < 3; i++)
                sem_post(&sem_elves);
        }
        if(reindeer_waiting == 9){
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(rand() % 3 + 2);
            ++presents_delivered;
            reindeer_waiting = 0;
            for(int i = 0; i < 9; i++)
                sem_post(&sem_reindeer);
        }
        printf("Mikołaj: zasypiam\n");
        pthread_mutex_unlock(&mutex);
    }
    for(int i = 0; i < N_ELVES; i++){
        pthread_cancel(elf_threads[i]);
        pthread_join(elf_threads[i], NULL);
    }
    for(int i = 0; i < N_REINDEER; i++){
        pthread_cancel(reindeer_threads[i]);
        pthread_join(reindeer_threads[i], NULL);
    }
    exit(EXIT_SUCCESS);
}

int main(void){
    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, santa, NULL);

    for(intptr_t i = 0; i < N_ELVES; i++){
        pthread_create(&elf_threads[i], NULL, elf, (void *)i);
    }
    for(intptr_t i = 0; i < N_REINDEER; i++){
        pthread_create(&reindeer_threads[i], NULL, reindeer, (void *)i);
    }

    pthread_join(santa_thread, NULL);
    return EXIT_SUCCESS;
}