#include "wc_lib.h"

#define INPUT_BUFF_SIZE 1024

typedef enum {
    init,   // init size - create new array with size size
    count,  // count file - count lines, words and characters in the file and save result in array
    show,   //
    delete,
    destroy,
    exit
} Command;

