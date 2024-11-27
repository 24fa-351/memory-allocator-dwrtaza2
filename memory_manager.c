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
    FreeListNode* current = free_list;
    FreeListNode* previous = NULL;

    while (current) {
        if (current->size >= size) {
            if (current->size > size + sizeof(FreeListNode)) {
                // Split the block
                FreeListNode* new_node = (FreeListNode*)((char*)current + sizeof(FreeListNode) + size);
                new_node->size = current->size - size - sizeof(FreeListNode);
                new_node->next = current->next;
                current->size = size;
                current->next = new_node;
            }

            if (previous) {
                previous->next = current->next;
            } else {
                free_list = current->next;
            }

            return (char*)current + sizeof(FreeListNode);
        }

        previous = current;
        current = current->next;
    }

    return NULL; // No suitable block found
}

void my_free(void* ptr) {
    if (!ptr) return;

    FreeListNode* node = (FreeListNode*)((char*)ptr - sizeof(FreeListNode));
    node->next = free_list;
    free_list = node;
}

void* my_realloc(void* ptr, size_t size) {
    if (!ptr) return my_malloc(size);

    FreeListNode* node = (FreeListNode*)((char*)ptr - sizeof(FreeListNode));
    if (node->size >= size) {
        return ptr;
    }

    void* new_ptr = my_malloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, node->size);
        my_free(ptr);
    }

    return new_ptr;
}
