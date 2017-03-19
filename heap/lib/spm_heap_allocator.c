#include <math.h>
#include "malloc.h"

#define ALIGNMENT 4

extern char _cacheable_heap_begin, _cacheable_heap_end;
extern char _spm_heap_begin, _spm_heap_end;

static size_t _pos = 0;

void * _allocate(size_t size) {
    char *addr = (char *)&_cacheable_heap_begin + _pos;
    _pos += (size_t)(ceil((double)size / ALIGNMENT) * ALIGNMENT);
    //fprintf(stderr, "%s, addr = %p, size = %lu\n", __func__, addr, size);
    return (void *)addr;
}

void * _allocate_in_bound(size_t size, size_t bound) {
    // If there is available SPM space, allocate memory space from a noncacheable region
    if (_pos + size <= bound) {
	//char *addr = (char *)&_spm_heap_begin + _pos;
	_pos += (size_t)(ceil((double)size / ALIGNMENT) * ALIGNMENT);
	//fprintf(stderr, "%s, addr = %p, size = %lu\n", __func__, addr, size);
	//return (void *)addr;
	return malloc(size);
    }
    // If there is not available SPM space, allocate memory space from a cacheable region
    return _allocate(size);
}


