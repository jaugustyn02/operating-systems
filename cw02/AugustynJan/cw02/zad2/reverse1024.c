#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rus_time.h"

#define SUCCESS 0
#define FAILURE 1
#define BUFF_SIZE 1024


int main(int argc, char* argv[]){
    time_start();

    // Arguments validation

    if(argc != 3){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }
    FILE *source_file = fopen(argv[1], "r");
    if (source_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open source file: %s\n", argv[3]);
        return FAILURE;
    }
    FILE *dest_file = fopen(argv[2], "w");

    // Reading source file and writing result to destination file

    char buff[BUFF_SIZE];
    size_t buff_len;
    fseek(source_file, (long)(-BUFF_SIZE), SEEK_END);
    while ((buff_len = (size_t)fread(buff, sizeof(char), (size_t)BUFF_SIZE, source_file)) > 0){
        for(int i=buff_len-1; i>=0; i--)
            fwrite(&buff[i], sizeof(char), (size_t)1, dest_file);
        if(ftell(source_file) <= (long)BUFF_SIZE) break;
        fseek(source_file, (long)(-2*BUFF_SIZE), SEEK_CUR);
    }

    fclose(source_file);
    fclose(dest_file);

    time_end();

    char* file_name = "reverse1024.exe";
    char* top_text = malloc((strlen(file_name)+ strlen(argv[1]) + strlen(argv[2]) + 3)*sizeof(char));
    sprintf(top_text, "%s %s %s", file_name, argv[1], argv[2]);
    printf("%s", top_text);
    write_time("pomiar_zad_2.txt", top_text, "", 1);
    free(top_text);

    return SUCCESS;
}