#include "allocator.h"
#include <c++/11/bits/fs_fwd.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
	bool alloced;
	size_t parent_index;
} chunk_t;


#define MEM_CHUNKS 1024
static chunk_t mem_chunks[MEM_CHUNKS] = {0};
static long memory[MEM_CHUNKS] = {0};

void* alloc(size_t bytes) {
	size_t alloc_chunks = bytes / 8;
	if (bytes % 8 > 0) {
		alloc_chunks++;
	}

	long offset = -1;
	size_t free_chunks = 0;
	for(size_t i = 0; i < MEM_CHUNKS; i++) {
		if (!mem_chunks[i].alloced && free_chunks == 0) {
			offset = i;
			free_chunks++;
		} else if (mem_chunks[i].alloced) {
			offset = -1;
			free_chunks = 0;
		} else {
			free_chunks++;
		}

		if (free_chunks == alloc_chunks) {
			break;
		}
	}

	if (offset == -1) {
		return NULL;
	}

	for(size_t i = offset; (i - offset) < alloc_chunks; i++) {
		mem_chunks[i].alloced = true;
		mem_chunks[i].parent_index = offset;
	}

	return (void*)memory + offset;
}

void free(void* ptr) {
	size_t offset = ptr-(void*)memory;

	size_t i = offset;
	while (mem_chunks[i].alloced && mem_chunks[i].parent_index == offset) {
		mem_chunks[i].alloced = false;
		mem_chunks[i].parent_index = 0;
		memory[i] = 0;
		i++;
	}
}

int main() {
	printf("memory: %p, size: %zu\n", memory, sizeof(memory));

	void* mem1 = alloc(24);
	alloc(20);
	free(mem1);
	alloc(18);
	void* mem = alloc(16);
	alloc(8);
	free(mem);
	alloc(34);
}
