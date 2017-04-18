#ifndef __CODE_MANAGER_H__
#define __CODE_MANAGER_H__

typedef struct _Region {
    char *func_lma;
} Region;

typedef struct _Map {
    char *func_lma;
    char *func_vma;
    unsigned long func_size;
    Region *reg;
} Map;

int c_init_regions(int num); 
int c_init_map(int num, ...);
char* c_get(char *func_lma);

extern Region *_region_table;
extern Map *_mapping_table;
extern int mapping_table_size;
extern int region_table_size;

#endif
