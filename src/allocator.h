#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void* alloc(size_t bytes);
void free(void* ptr);

#endif // !ALLOCATOR_H
