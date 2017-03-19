#include <math.h>
#include "malloc.h"

#define ALIGNMENT 4

extern char _cacheable_heap_begin, _cacheable_heap_end;

static char *_pos = &_cacheable_heap_begin;

void * _allocate(size_t size) {
    void *addr = (void *)_pos;
    _pos += (long)(ceil((double)size / ALIGNMENT) * ALIGNMENT);
    return addr;
}



