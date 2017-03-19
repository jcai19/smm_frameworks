#ifndef __SPM_HEAP_ALLOCATOR_H__
#define __SPM_HEAP_ALLOCATOR_H__

#include <stddef.h>


void* _allocate(size_t size);
void* _allocate_in_bound(size_t size, size_t bound);

#endif
