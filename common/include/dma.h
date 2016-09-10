#ifndef __DMA_H__
#define __DMA_H__

#include <stdlib.h>

enum TRANSDIR {SPM2MEM, MEM2SPM};

#if defined(__i386__)

//int dma(void *, void *, unsigned int, int) __attribute__((always_inline));
int dma(void *, void *, unsigned int, int);

#elif defined(__x86_64__)

//int dma(void *, void *, unsigned long, int) __attribute__((always_inline));
int dma(void *, void *, unsigned long, int);

#endif


#endif
