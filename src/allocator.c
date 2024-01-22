#include "allocator.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>

#define CHUNK_SIZE 8
#define CHUNK_NUM 1024
#define MEMORY_CAP 640000

typedef struct CHUNK {
  char *start;
  size_t size;
} chunk_t;

typedef struct {
  size_t count;
  chunk_t chunks[CHUNK_NUM];
} chunk_list_t;

char memory[MEMORY_CAP] = {0};
chunk_list_t free_chunks = {
    .count = 1, .chunks = {[0] = {.start = memory, .size = MEMORY_CAP}}};
chunk_list_t alloced_chunks = {.count = 0, .chunks = {0}};

void chunk_list_dump(const chunk_list_t *list) {
  printf("Chunks: %zu\n", list->count);
  for (size_t i = 0; i < list->count; i++) {
    const chunk_t current = list->chunks[i];
    printf("#%zu: start: %p, size: %zu\n", i + 1, current.start, current.size);
  }
}

void chunk_list_remove(chunk_list_t *list, size_t index) {
  while (index < list->count) {
    memmove(&list->chunks[index], &list->chunks[index + 1], sizeof(chunk_t));
    index += 1;
  }
  list->count -= 1;
}

void *alloc(size_t bytes) {
  if (bytes > 0) {
    size_t chunks = bytes / CHUNK_SIZE;

    if (bytes % CHUNK_SIZE > 0)
      chunks += 1;
    size_t alloced_size = chunks * CHUNK_SIZE;

    for (size_t i = 0; i < free_chunks.count; i++) {
      chunk_t current = free_chunks.chunks[i];
      if (current.size >= alloced_size) {
        void *ptr = current.start;
        alloced_chunks.chunks[alloced_chunks.count].start = current.start;
        alloced_chunks.chunks[alloced_chunks.count].size = alloced_size;
        alloced_chunks.count += 1;

        if (current.size > alloced_size) {
          size_t tail = current.size - alloced_size;
          free_chunks.chunks[i].start = current.start + alloced_size;
          free_chunks.chunks[i].size = tail;
        } else {
          chunk_list_remove(&free_chunks, i);
        }

        return ptr;
      }
    }
  }

  return NULL;
}

void free(void *ptr) { printf("Free %p\n", ptr); }

int main() {
  for (size_t i = 1; i < 20; i++) {
    void *mem = alloc(i);
    if (i % 2 == 0)
      free(mem);
  }

  chunk_list_dump(&alloced_chunks);
  chunk_list_dump(&free_chunks);
}
