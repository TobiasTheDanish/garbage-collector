#include "allocator.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

typedef struct CHUNK {
  void *start;
  size_t size;
  struct CHUNK *next;
} chunk_t;

static size_t CHUNK_SIZE = 8;
static size_t CHUNK_NUM = 1024;
static chunk_t *free_chunks = NULL;

void init() {
  void *start = mmap(NULL, CHUNK_NUM, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if ((void *)-1 == start) {
    printf("Could not map memory: %s\n", strerror(errno));
  }

  chunk_t root = {
      .start = start,
      .size = CHUNK_NUM,
      .next = NULL,
  };

  free_chunks = mmap(NULL, sizeof(chunk_t), PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if ((void *)-1 == free_chunks) {
    printf("Could not map memory: %s\n", strerror(errno));
  }

  printf("free_chunks: %p, root: %p, root->start: %p\n", free_chunks, &root,
         root.start);

  memcpy(free_chunks, &root, sizeof(chunk_t));
}

void *alloc(size_t bytes) {
  if (free_chunks == NULL) {
    init();
  }

  chunk_t current = free_chunks[0];

  while (1) {
    if (bytes > current.size) {
      if (current.next == NULL) {
        printf("[ERROR]: Could not find a fitting block to allocate to\n");
        exit(1);
      }

      current = *current.next;
    } else {
      printf("We found a chunk big enough\n");
      break;
    }
  }

  return current.start;
}

void free(void *ptr) { printf("Free %p\n", ptr); }

int main() {
  void *mem1 = alloc(24);
  free(mem1);
}
