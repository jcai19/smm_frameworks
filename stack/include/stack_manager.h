#ifndef __STACK_MANAGER_H__
#define __STACK_MANAGER_H__

#include <stddef.h>

#define getSP(sp) { __asm __volatile("mov %%rsp, %0;\n":"=m"(sp));}
#define putSP(sp) { __asm __volatile("mov %0, %%rsp;\n"::"m"(sp):"%rsp");}

typedef struct {
	char *spm_addr; // location in the SPM right before eviction
	char *mem_addr; // location in the main memory right after eviction
	unsigned long long ssize; // size of stack frames being evicted
	int status; // --1: resides in SPM; --0 : resides in memory
} STACK_ENTRY;

extern char _spm_stack_begin, _spm_stack_end;
extern char *_spm_stack_base, *_mem_stack_base;
extern long long int _mem_stack_depth;
extern STACK_ENTRY _mem_stack[20];
extern char* _stack_pointer;

void _sstore() __attribute__((noinline));
void _sload() __attribute__((noinline));

char * _l2g(char *) __attribute__((noinline));
char * _g2l(char *, unsigned long) __attribute__((noinline));
//void _ptr_wr(char *, unsigned long) __attribute__((noinline));

#endif
