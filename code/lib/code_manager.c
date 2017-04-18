#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code_manager.h"
#include "dma.h"

Region *_region_table;
Map *_mapping_table;
int mapping_table_size;
int region_table_size;

//extern long dmaCount;

int c_init_regions(int num) {
    int i;

    region_table_size = num;
    _region_table = (Region *)malloc(sizeof(Region) * num);
    for(i=0; i<num; i++) {
	_region_table[i].func_lma = NULL;
    }

    return 0;
}

int c_init_map(int num, ...) {
    int i;
    va_list vl;
    va_start(vl, num);

    mapping_table_size = num;
    _mapping_table = (Map *)malloc(sizeof(Map) * num);
    for (i=0; i<num; i++) {
	//_mapping_table[i].func_name = va_arg(vl, const char*);
	_mapping_table[i].func_lma = va_arg(vl, char *);
	_mapping_table[i].func_vma = va_arg(vl, char *);
	_mapping_table[i].func_size = va_arg(vl, unsigned long);
	_mapping_table[i].reg = va_arg(vl, Region *);
    }
    va_end(vl);
    return 0;
}

char* c_get(char *func_lma) {
    int i;
    Map *map;
    for(i=0; i<mapping_table_size; i++) {
	map = &_mapping_table[i];
	//if(!strcmp(func_name, map->func_name)) break;
	if(func_lma == map->func_lma) 
	    break;
    }

    if(i >= mapping_table_size) {
	printf("Cannot find mapping for %s\n", func_lma);
	exit(-1);
    }

    if(func_lma != map->reg->func_lma) {
	//printf("Before DMA, func_name=%s, map->reg->func_name=%s, map->func_vma=%lx, map_func_lma=%lx, dmaCount=%ld\n", func_name, map->reg->func_name, (unsigned long)map->func_vma, (unsigned long)map->func_lma, dmaCount);
	//++dmaCount;
	dma(map->func_vma, map->func_lma, map->func_size, MEM2SPM);
	map->reg->func_lma = func_lma;
	//printf("After DMA, func_name=%s, map->reg->func_name=%s, map->func_vma=%lx, map_func_lma=%lx, dmaCount=%ld\n", func_name, map->reg->func_name, (unsigned long)map->func_vma, (unsigned long)map->func_lam, dmaCount);
	//printf("After DMA, func_name=%s, map->reg->func_name=%s, map->func_vma=%lx, map_func_lma=%lx, dmaCount=%ld\n", func_name, map->reg->func_name, (unsigned long)map->func_vma, (unsigned long)map->func_lma, dmaCount);
    }
    return map->func_vma;
}
