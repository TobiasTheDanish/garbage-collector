#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void *alloc(size_t bytes) asm("_alloc");
void free(void *ptr) asm("_free");

#endif // !ALLOCATOR_H
