#ifndef __CODE_MANAGER_H__
#define __CODE_MANAGER_H__

typedef struct _Region {
    char *func_name;
} Region;

typedef struct _Map {
    const char *func_name;
    char *func_lma;
    char *func_vma;
    unsigned long func_size;
    Region *reg;
} Map;

int c_init(const char *mapping_file_name);
int c_init_reg(int num); 
int c_init_map(int num, ...);
char* c_get(char *func_name);

extern Region *_region_table;
extern Map *_mapping_table;
extern int mapping_table_size;
extern int region_table_size;

#endif
