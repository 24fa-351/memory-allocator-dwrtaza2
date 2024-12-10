#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory_manger.h"

void test_malloc_free() {
    printf("Running test_malloc_free...\n");
    void* ptr = my_malloc(128);
    if (!ptr) {
        printf("Failed: my_malloc returned NULL\n");
        return;
    }
    memset(ptr, 0, 128); // Ensure memory is initialized to zero
    my_free(ptr);
    printf("Passed\n");
}

void test_realloc() {
    printf("Running test_realloc...\n");
    void* ptr = my_malloc(64);
    if (!ptr) {
        printf("Failed: my_malloc returned NULL\n");
        return;
    }
    memset(ptr, 0xAA, 64); // Set a test pattern
    ptr = my_realloc(ptr, 128);
    if (!ptr) {
        printf("Failed: my_realloc returned NULL\n");
        return;
    }
    my_free(ptr);
    printf("Passed\n");
}

int main(int argc, char* argv[]) {
    memory_manager_init(1024 * 1024); // Allocate 1MB for custom heap
    test_malloc_free();
    test_realloc();
    memory_manager_cleanup();
    return 0;
}
