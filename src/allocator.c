#include "allocator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define CHUNK_SIZE 8
#define CHUNK_NUM 1024
#define MEMORY_CAP 640000

typedef struct CHUNK {
  uintptr_t *start;
  size_t size;
} chunk_t;

typedef struct {
  size_t count;
  chunk_t chunks[CHUNK_NUM];
} chunk_list_t;

uintptr_t memory[MEMORY_CAP] = {0};
chunk_list_t free_chunks = {
    .count = 1, .chunks = {[0] = {.start = memory, .size = MEMORY_CAP}}};

chunk_list_t alloced_chunks = {.count = 0};

void chunk_list_dump(const chunk_list_t *list) {
  printf("Chunks: %zu\n", list->count);
  for (size_t i = 0; i < list->count; i++) {
    const chunk_t current = list->chunks[i];
    printf("#%zu: start: %p, size: %zu\n", i + 1, current.start, current.size);
  }
}

int chunk_list_insert(chunk_list_t *list, uintptr_t *start, size_t size) {
  list->chunks[list->count].start = start;
  list->chunks[list->count].size = size;

  size_t i = list->count;
  for (; i > 0 && list->chunks[i - 1].start > list->chunks[i].start; i--) {
    const chunk_t tmp = {.start = list->chunks[i].start,
                         .size = list->chunks[i].size};

    list->chunks[i].start = list->chunks[i - 1].start;
    list->chunks[i].size = list->chunks[i - 1].size;

    list->chunks[i - 1].start = tmp.start;
    list->chunks[i - 1].size = tmp.size;
  }

  list->count += 1;

  return i;
}

void chunk_list_remove(chunk_list_t *list, size_t index) {
  while (index < list->count - 1) {
    memmove(&list->chunks[index], &list->chunks[index + 1], sizeof(chunk_t));
    index += 1;
  }
  list->count -= 1;
}

void chunk_list_try_merge(chunk_list_t *list, size_t index) {
  if (index > 0) {
    const chunk_t current = list->chunks[index];
    const chunk_t prev = list->chunks[index - 1];
    if (prev.start + prev.size == current.start) {
      list->chunks[index - 1].size += current.size;
      chunk_list_remove(list, index);
      index -= 1;
    }
  }

  if (index < list->count - 1) {
    const chunk_t current = list->chunks[index];
    const chunk_t next = list->chunks[index + 1];
    if (current.start + current.size == next.start) {
      list->chunks[index].size += next.size;
      chunk_list_remove(list, index + 1);
    }
  }
}

int chunk_list_b_search(const chunk_list_t *list, uintptr_t *ptr) {
  if (list->count == 0) {
    return -1;
  }

  size_t min = 0;
  size_t max = list->count - 1;
  float diff = max - min;
  int index = min + (diff / 2);

  while (index >= 0 && index < (int)list->count) {
    if (list->chunks[index].start == ptr) {
      return index;
    } else if (list->chunks[index].start > ptr) {
      max = index;
    } else if (list->chunks[index].start < ptr) {
      min = index;
    }

    diff = max - min;
    index = min + (diff / 2);
  }

  return -1;
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
        chunk_list_insert(&alloced_chunks, ptr, alloced_size);

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

    printf("[ERROR]: Heap out of memory. Attempt to alloc %zu bytes\n", bytes);
    printf("Alloced ");
    chunk_list_dump(&alloced_chunks);
    printf("Freed ");
    chunk_list_dump(&free_chunks);
    exit(1);
  }

  return NULL;
}

void free(void *ptr) {
  if (ptr == NULL)
    return;

  uintptr_t *uptr = (uintptr_t *)ptr;

  int index = chunk_list_b_search(&alloced_chunks, uptr);
  if (index < 0) {
    printf("[ERROR]: Did not find a matching ptr\n");
    exit(1);
  }
  if (alloced_chunks.chunks[index].start != uptr) {
    printf("[ERROR]: Found chunk did not match ptr\n");
    exit(1);
  }

  const chunk_t chunk_to_free = {.start = alloced_chunks.chunks[index].start,
                                 .size = alloced_chunks.chunks[index].size};

  int place =
      chunk_list_insert(&free_chunks, chunk_to_free.start, chunk_to_free.size);
  chunk_list_try_merge(&free_chunks, place);

  chunk_list_remove(&alloced_chunks, index);
}

int main() {
  size_t ptrs_count = 10;
  void *ptrs[ptrs_count];
  for (size_t i = 0; i < ptrs_count; i++) {
    ptrs[i] = alloc((i + 1) * 8);
  }

  for (size_t i = 0; i < ptrs_count; i++) {
    if (i % 4 == 0) {
      free(ptrs[i]);
    }
  }

  printf("Alloced ");
  chunk_list_dump(&alloced_chunks);
  printf("Freed ");
  chunk_list_dump(&free_chunks);
}
