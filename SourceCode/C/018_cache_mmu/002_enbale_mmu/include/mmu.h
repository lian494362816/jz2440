#ifndef __MMU_H__
#define __MMU_H__

enum CACHE_MODE
{
    E_CACHE_NCNB = 0, /* No cache, No write buff */
    E_CACHE_NCB = 1,  /* No Cache, Use write buff */
    E_CACHE_WT = 2,   /* Use Cache, write-through mode */
    E_CACHE_WB = 3,   /* Use Cache, write-back mode */
};

void mmu_section_map(unsigned int *ttb, unsigned int va, unsigned int pa, enum CACHE_MODE mode);
void mmu_create_table(void);


#endif
