#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <stdint.h>

// Define the size of the memory arena
#define ARENA_SIZE (1024 * 1024) // 1MB arena

// Memory Arena structure
typedef struct
{
    uint8_t *buffer; // The memory block
    size_t capacity; // Total size of the buffer
    size_t offset;   // Current position in the buffer
} MemoryArena;

// Functions for managing the Memory Arena
void init_arena(MemoryArena *arena, size_t size);
void *arena_alloc(MemoryArena *arena, size_t size);
void arena_reset(MemoryArena *arena);
void free_arena(MemoryArena *arena);

#endif // ARENA_H