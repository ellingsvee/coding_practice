#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "memoryarena.h"

void init_arena(MemoryArena *arena, size_t size)
{
    arena->buffer = (uint8_t *)malloc(size);
    if (!arena->buffer)
    {
        fprintf(stderr, "Failed to allocate memory arena\n");
        exit(EXIT_FAILURE);
    }
    arena->capacity = size;
    arena->offset = 0;
}

void *arena_alloc(MemoryArena *arena, size_t size)
{
    if (arena->offset + size > arena->capacity)
    {
        fprintf(stderr, "Arena out of memory\n");
        return NULL;
    }
    void *ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

void arena_reset(MemoryArena *arena)
{
    arena->offset = 0;
}

void free_arena(MemoryArena *arena)
{
    free(arena->buffer);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}