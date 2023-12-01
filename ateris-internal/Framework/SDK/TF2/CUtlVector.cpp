#include "CUtlVector.h"
#include <corecrt_malloc.h>

void* why::gameAlloc( size_t size )
{
    return malloc( size );//i::MemAlloc->Alloc(size);
}

void* why::gameRealloc( void* mem, size_t size )
{
    return realloc( mem, size );//i::MemAlloc->Realloc(mem, size);
}

void why::gameFree( void* mem )
{
    return free( mem );//i::MemAlloc->Free(mem);
}