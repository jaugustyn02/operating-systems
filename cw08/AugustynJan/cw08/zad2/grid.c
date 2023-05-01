#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <stdio.h>

#include "grid.h"

#define GRID_WIDTH 30
#define GRID_HEIGHT 30

const int grid_width = GRID_WIDTH;
const int grid_height = GRID_HEIGHT;
int n_threads = 0;

pthread_t *threads;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

// Step simulation - sending SIGUSR1 to all threads
void update_grid(){
    for (int i=0; i<n_threads; i++)
        pthread_kill(threads[i] , SIGUSR1);
}

// struct for passing arguments to thread
struct args_buf{
    int start;
    int end;
    char *src;
    char *dst;
};

// thread function
void* update_cells(void *arg){
    struct args_buf *args = (struct args_buf*)arg;
    int start = args->start;
    int end = args->end;
    char *src = args->src;
    char *dst = args->dst;
    free(args);

    while (true) {
        for (int i = start; i < end; i++) {
            int row = i / grid_width;
            int col = i % grid_width;
            dst[i] = is_alive(row, col, src);
        }

        pause();

        char *tmp = src;
        src = dst;
        dst = tmp;
    }
    return NULL;
}

void SIGUSR1_handler(int signum){}

void init_threads(int _n_threads, char *src, char *dst){
    n_threads = _n_threads;
    threads = malloc(sizeof(pthread_t) * n_threads);

    signal(SIGUSR1, SIGUSR1_handler); // ignore SIGUSR1

    int grid_size = grid_width * grid_height;
    int n_cells_per_thread = grid_size / n_threads;
    int n_cells_left = grid_size % n_threads;

    int end = 0;
    for(int i = 0; i < n_threads; i++){
        struct args_buf *args = malloc(sizeof(struct args_buf));
        args->start = end;
        args->end = args->start + n_cells_per_thread + (i < n_cells_left ? 1 : 0);
        end = args->end;
        args->src = src;
        args->dst = dst;
        if (pthread_create(&threads[i], NULL, &update_cells, (void *)args) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
}

void cleanup_threads(){
    for (int i = 0; i < grid_height; i++)
        for (int j = 0; j < grid_width; j++)
            pthread_cancel(threads[i * grid_width + j]);

    free(threads);
}