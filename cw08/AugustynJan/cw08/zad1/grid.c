#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "grid.h"

#define GRID_WIDTH 30
#define GRID_HEIGHT 30

const int grid_width = GRID_WIDTH;
const int grid_height = GRID_HEIGHT;

pthread_t cell_threads[GRID_WIDTH * GRID_HEIGHT];

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

// Simulation step - send SIGUSR1 to all threads
void update_grid(){
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            pthread_kill(cell_threads[i*grid_width + j] , SIGUSR1);
        }
    }
}

// Struct for passing arguments to thread
struct args_buf{
    int row;
    int col;
    char *src;
    char *dst;
};

// Thread function
void* update_cell(void *arg){
    struct args_buf *args = (struct args_buf*)arg;
    int row = args->row;
    int col = args->col;
    char *src = args->src;
    char *dst = args->dst;
    free(args);

    while (true) {
        dst[row * grid_width + col] = is_alive(row, col, src);

        pause();

        char *tmp = src;
        src = dst;
        dst = tmp;
    }
    return NULL;
}

void SIGUSR1_handler(int signum){}

void init_threads(char *src, char *dst){
    signal(SIGUSR1, SIGUSR1_handler);

    for (int i = 0; i < grid_height; i++)
    {
        for (int j = 0; j < grid_width; j++)
        {
            struct args_buf *args = malloc(sizeof(struct args_buf));
            args->row = i;
            args->col = j;
            args->src = src;
            args->dst = dst;
            if (pthread_create(&cell_threads[i * grid_width + j], NULL, &update_cell, (void *)args) != 0)
            {
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void cleanup_threads(){
    for (int i = 0; i < grid_height; i++)
        for (int j = 0; j < grid_width; j++)
            pthread_cancel(cell_threads[i * grid_width + j]);
}