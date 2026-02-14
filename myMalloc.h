#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stddef.h>

void *myMalloc(size_t size);
void freeMem(void *memBlock);

#endif
