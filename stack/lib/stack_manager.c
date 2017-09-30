#include <string.h>
#include <stddef.h>

#include "stack_manager.h"
#include "dma.h"

// stack frame management

char *_spm_stack_base, *_mem_stack_base;
long long int _mem_stack_depth = 0;
STACK_ENTRY _mem_stack[20];

char buf1[128], buf2[128];

// pointer management
char *_stack_pointer;
static char *gaddr, *laddr;

// copy stack frames from SPM to main memory
void _sstore() {
    // move caller stack frame from SPM to memory
    _mem_stack[_mem_stack_depth].spm_addr = _stack_pointer; // store current value of the stack pointer (compiler should have inserted getSP(_stack_pointer) right before _sstore)
    _mem_stack[_mem_stack_depth].ssize = _spm_stack_base - _mem_stack[_mem_stack_depth].spm_addr; // calculate the size of the caller stack frame

    if (_mem_stack_depth == 0) { // call stack is empty
	_mem_stack[_mem_stack_depth].mem_addr = _mem_stack_base - _mem_stack[_mem_stack_depth].ssize; // reserve space for caller stack frame in memory
    }
    else { // call stack is not empty
	_mem_stack[_mem_stack_depth].mem_addr = _mem_stack[_mem_stack_depth-1].mem_addr - _mem_stack[_mem_stack_depth].ssize; // calculate the size of the caller stack frame
    }
    _mem_stack[_mem_stack_depth].status = 0; // update status of function stack to indicate it is in memory
    dma(_mem_stack[_mem_stack_depth].spm_addr, _mem_stack[_mem_stack_depth].mem_addr, _mem_stack[_mem_stack_depth].ssize, SPM2MEM);	// copy caller stack frame from SPM to memory
    _mem_stack_depth++; // increase stack depth counter
}

// copy stack frames from the main memory to SPM
void _sload() {
    // bring back caller stack frame from memory to SPM
    _mem_stack_depth--; // decrease stack depth counter
    dma((void *)_mem_stack[_mem_stack_depth].spm_addr, (void *)_mem_stack[_mem_stack_depth].mem_addr, _mem_stack[_mem_stack_depth].ssize, MEM2SPM);// copy back caller stack frame from memory to SPM
}

// translate an SPM address to a main memory address
char * _l2g(char *laddr)
{
    // do address translation only if the address passed in is in stack in the SPM
    gaddr = laddr; // set return value to the passed in address by default
    // calculate the offset from the beginning of the current stack frame to the address
    if (_mem_stack_depth == 0) {
	if (laddr >= _stack_pointer && laddr < _spm_stack_base) {
	    gaddr = _mem_stack_base - (_spm_stack_base - laddr);
	}
    } else {
	if (laddr >= _stack_pointer && laddr < _spm_stack_base) {
	    gaddr = _mem_stack[_mem_stack_depth-1].mem_addr - (_spm_stack_base - laddr);
	}
    }
    return gaddr;
}

// translate a main memory address to an SPM address
char * _g2l(char *gaddr, unsigned long size) {
    // do address translation only if the address passed in is in stack in the main memory
    laddr = gaddr; // set return value to the passed in address by default
    if (_mem_stack_depth == 0) {
	if (gaddr >= _mem_stack_base - (_spm_stack_base - _stack_pointer) && gaddr < _mem_stack_base) {
	    laddr = _spm_stack_base - (_mem_stack_base - gaddr);
	    //dma((void *)buf1, (void *)gaddr, size, MEM2SPM);
	    //laddr = buf1;
	    dma((void *)buf1, (void *)buf2, size, MEM2SPM);
	}
    } else {
	if (gaddr >= _mem_stack[_mem_stack_depth-1].mem_addr - (_spm_stack_base - _stack_pointer) && gaddr < _mem_stack[_mem_stack_depth-1].mem_addr) {
	    laddr = _spm_stack_base - (_mem_stack[_mem_stack_depth-1].mem_addr - gaddr);
	    //dma((void *)buf1, (void *)gaddr, size, MEM2SPM);
	    //laddr = buf1;
	    dma((void *)buf1, (void *)buf2, size, MEM2SPM);
	}
    }

    return laddr;
}

void _ptr_wr(char *gaddr, unsigned long size) {
    if (_mem_stack_depth == 0) {
	if (gaddr >= _mem_stack_base - (_spm_stack_base - _stack_pointer) && gaddr < _mem_stack_base) {
	    laddr = _spm_stack_base - (_mem_stack_base - gaddr);
	    // TODO writes to the main memory
	    dma((void *)buf1, (void *)buf2, size, SPM2MEM);
	} 
    } else {
	if (gaddr >= _mem_stack[_mem_stack_depth-1].mem_addr - (_spm_stack_base - _stack_pointer) && gaddr < _mem_stack[_mem_stack_depth-1].mem_addr) {
	    laddr = _spm_stack_base - (_mem_stack[_mem_stack_depth-1].mem_addr - gaddr);
	    // TODO writes to the main memory
	    dma((void *)buf1, (void *)buf2, size, SPM2MEM);
	} 
    }
}
