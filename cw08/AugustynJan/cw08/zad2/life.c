#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "grid.h"

char *foreground;
char *background;

void cleanup();

int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("Usage: %s <number of threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n_threads = atoi(argv[1]);
    if (n_threads <= 0){
        printf("Number of threads must be positive\n");
        exit(EXIT_FAILURE);
    }

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

    foreground = create_grid();
    background = create_grid();

	init_grid(foreground);
    init_threads(n_threads, foreground, background);

    atexit(cleanup);

	while (true)
	{
		draw_grid(foreground);
        usleep(500 * 1000);

        // Step simulation
        update_grid();

		char *tmp = foreground;
		foreground = background;
		background = tmp;

	}
}

void cleanup()
{
    cleanup_threads();
    endwin(); // End curses mode
    destroy_grid(foreground);
    destroy_grid(background);
}