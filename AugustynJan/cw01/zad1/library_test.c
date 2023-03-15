#include "wc_lib.h"
#include <stdio.h>

int main(int argc, char **argv){
    if (argc > 0) {
        Memory memory = Memory_create(20);
        Memory_add(&memory, "wc_lib.c");
        Memory_add(&memory, "wc_lib.h");
        printf("%s", Memory_get(&memory, 0));
        printf("%s", Memory_get(&memory, 1));
        Memory_remove(&memory, 0);
        printf("%s", Memory_get(&memory, 0));
        Memory_clear(&memory);
    }
}