#include <stdio.h>
#include <string.h>
#include "rus_time.h"

#define SUCCESS 0
#define FAILURE 1


int main(int argc, char* argv[]){
    time_start();

    // Arguments validation

    if(argc != 5){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }
    if(strlen(argv[1]) != 1){
        fprintf(stderr, "Error: Invalid first argument\n");
        return FAILURE;
    }
    if(strlen(argv[2]) != 1){
        fprintf(stderr, "Error: Invalid second argument\n");
        return FAILURE;
    }
    const char from=(char)*argv[1], to=(char)*argv[2];

    FILE *source_file = fopen(argv[3], "r");
    if (source_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open source file: %s\n", argv[3]);
        return FAILURE;
    }
    FILE *dest_file = fopen(argv[4], "w");

    // Reading source file and writing result to destination file

    char c;
    while (fread(&c, (size_t)1, (size_t)1, source_file) == 1){
        if(c == from) c = to;
        fwrite(&c, (size_t)1, (size_t)1, dest_file);
    }

    fclose(source_file);
    fclose(dest_file);

    time_end();
    write_time("pomiar_zad_1.txt", "tr_lib.exe", "", 0);

    return SUCCESS;
}