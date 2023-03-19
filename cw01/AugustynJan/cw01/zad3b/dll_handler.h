#ifndef DLL_HANDLER_H
#define DLL_HANDLER_H

#include <stdio.h>
#include <dlfcn.h>

#include "libwc_dynamic.h"

#define LIB_FILENAME "libwc.so"
#define SUCCESS 0
#define FAILURE 1

void *handle;

int load_symbols(const char* file_path){

    handle = dlopen(file_path, RTLD_LAZY);

    if(handle == NULL) {
        fprintf(stderr, "Error: Dll not found");
        return FAILURE;
    }

    *(void **) (&Memory_create) = dlsym(handle, "Memory_create");
    if(dlerror() != NULL){
        fprintf(stderr, "Error: couldn't load method: Memory_create\n");
        return FAILURE;
    }

    *(void **) (&Memory_add) = dlsym(handle, "Memory_add");
    if(dlerror() != NULL){
        fprintf(stderr, "Error: couldn't load method: Memory_add\n");
        return FAILURE;
    }

    *(void **) (&Memory_get) = dlsym(handle, "Memory_get");
    if(dlerror() != NULL){
        fprintf(stderr, "Error: couldn't load method: Memory_get\n");
        return FAILURE;
    }

    *(void **) (&Memory_remove) = dlsym(handle, "Memory_remove");
    if(dlerror() != NULL) {
        fprintf(stderr, "Error: couldn't load method: Memory_remove\n");
        return FAILURE;
    }

    *(void **) (&Memory_clear) = dlsym(handle, "Memory_clear");
    if(dlerror() != NULL){
        fprintf(stderr, "Error: couldn't load method: Memory_clear\n");
        return FAILURE;
    }

    return SUCCESS;
}

void free_handle(){
    dlclose(handle);
}

#else
int load_symbols(void) {
    return FAILURE;
}
void free_handle(){};
#endif