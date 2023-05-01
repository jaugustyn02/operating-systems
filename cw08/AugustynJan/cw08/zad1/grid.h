#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid();
void* update_cell(void* arg);
void init_threads(char *src, char *dst);
void cleanup_threads();