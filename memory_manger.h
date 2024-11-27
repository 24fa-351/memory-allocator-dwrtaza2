#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

// Custom malloc, free, and realloc functions
void* my_malloc(size_t size);
void my_free(void* ptr);
void* my_realloc(void* ptr, size_t size);

// Initialization for the memory manager
void memory_manager_init(size_t heap_size);
void memory_manager_cleanup();

#endif
