#include <stdlib.h>

#if defined(__i386__)

int dma(void *spmAddr, void *memAddr, unsigned int size, int transDir) {
    int bytes_trans = 0;
    __asm__ __volatile__("mov %1, %%ebx\n\t"
	    "mov %2, %%ecx\n\t"
	    "mov %3, %%edx\n\t"
	    "mov %4, %%esi\n\t"
	    "mov $324, %%eax\n\t"
	    "int $0x80\n\t"
	    "mov %%eax, %0"
	    :"=m" (bytes_trans)
	    :"m"(spmAddr), "m"(memAddr), "m"(size), "m"(transDir)
	    :"%esi", "%edx", "%ecx", "%ebx", "%eax"
	    );
    return bytes_trans;
}


#elif defined(__x86_64__)

int dma(void *spmAddr, void *memAddr, unsigned long size, int transDir) {
    int bytes_trans;
    __asm__ __volatile__("mov %1, %%rdi\n\t"
	    "mov %2, %%rsi\n\t"
	    "mov %3, %%rdx\n\t"
	    "mov %4, %%r10\n\t"
	    "mov $273, %%eax\n\t"
	    "syscall\n\t"
	    "mov %%eax, %0"
	    :"=m" (bytes_trans)
	    :"m"(spmAddr), "m"(memAddr), "m"(size), "m"(transDir)
	    :"%rdi", "%rsi", "%rdx", "r10", "%eax"
	    );
    return bytes_trans;
}

#endif
