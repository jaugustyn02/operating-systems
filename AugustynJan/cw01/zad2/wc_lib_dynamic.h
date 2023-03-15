#ifndef WC_LIB_DYNAMIC_H
#define WC_LIB_DYNAMIC_H

#include <dlfcn.h>
#include "wc_lib.h"

Memory (*Memory_create) (size_t);
void (*Memory_add) (Memory*, char*);
char* (*Memory_get) (Memory*, size_t);
void (*Memory_remove) (Memory*, size_t);
void (*Memory_clear) (Memory*);

#endif // WC_LIB_DYNAMIC_H