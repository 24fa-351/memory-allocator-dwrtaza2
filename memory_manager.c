#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

// Custom malloc, free, and realloc functions
void* my_malloc(size_t size);
void my_free(void* pointer);
void* my_realloc(void* pointer, size_t size);

// Initialization for the memory manager
void memory_manager_init(size_t heap_size);
void memory_manager_cleanup();

#endif

#include "memory_manger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Global variables for heap memory
static void* heap_base = NULL;
static size_t heap_size = 0;

// Structure for free list node
typedef struct FreeListNode {
    size_t size;
    struct FreeListNode* next;
} FreeListNode;

static FreeListNode* free_list = NULL;

void memory_manager_init(size_t size) {
    heap_base = sbrk(size);
    if (heap_base == (void*)-1) {
        fprintf(stderr, "Failed to allocate memory using sbrk\n");
        exit(EXIT_FAILURE);
    }
    heap_size = size;
    memset(heap_base, 0, heap_size); // Reset to 0

    // Initialize free list with the entire heap
    free_list = (FreeListNode*)heap_base;
    free_list->size = heap_size - sizeof(FreeListNode);
    free_list->next = NULL;
}

void memory_manager_cleanup() {
    if (heap_base) {
        brk(heap_base); // Reset heap to initial state
    }
}

void* my_malloc(size_t size) {
    FreeListNode* current_node = free_list;
    FreeListNode* previous_node = NULL;

    while (current_node) {
        if (current_node->size >= size) {
            if (current_node->size > size + sizeof(FreeListNode)) {
                // Split the block
                FreeListNode* new_node = (FreeListNode*)((char*)current_node + sizeof(FreeListNode) + size);
                new_node->size = current_node->size - size - sizeof(FreeListNode);
                new_node->next = current_node->next;
                current_node->size = size;
                current_node->next = new_node;
            }

            if (previous_node) {
                previous_node->next = current_node->next;
            } else {
                free_list = current_node->next;
            }

            return (char*)current_node + sizeof(FreeListNode);
        }

        previous_node = current_node;
        current_node = current_node->next;
    }

    return NULL; // No suitable block found
}

void my_free(void* pointer) {
    if (!pointer) return;

    FreeListNode* node = (FreeListNode*)((char*)pointer - sizeof(FreeListNode));
    node->next = free_list;
    free_list = node;
}

void* my_realloc(void* pointer, size_t size) {
    if (!pointer) return my_malloc(size);

    FreeListNode* node = (FreeListNode*)((char*)pointer - sizeof(FreeListNode));
    if (node->size >= size) {
        return pointer;
    }

    void* new_pointer = my_malloc(size);
    if (new_pointer) {
        memcpy(new_pointer, pointer, node->size);
        my_free(pointer);
    }

    return new_pointer;
}
