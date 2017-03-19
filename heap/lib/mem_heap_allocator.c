#include <math.h>

#define ALIGNMENT 4

extern char _cacheable_heap_begin, _cacheable_heap_end;

static size_t _pos = 0;

void * _allocate(size_t size) {
    char *addr = (char *)&_cacheable_heap_begin + _pos;
    _pos += (size_t)(ceil((double)size / ALIGNMENT) * ALIGNMENT);
    return (void *)addr;
}


