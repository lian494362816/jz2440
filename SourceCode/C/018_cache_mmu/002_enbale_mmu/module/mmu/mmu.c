#include "mmu.h"

#define MMU_TTB_BASE (0x33A00000)

#define MMU_SECTION_AP (0x3 << 10) /* All access types permitted */
#define MMU_SECTION_DOMAIN (0x0 << 5) /* Domain 0 */
#define MMU_SECTION_FORCE_BIT ((0x1 << 4) | (0x1 << 1))

/*
    [31:20]               [19:12]   [11:10]  [9]      [8:5]   [4] [3] [2] [1] [0]
    Section base address: Not Use:  AP:      Not Use: Domain: 1:  C:  B:  1:  0
    Not Use should be zero
*/

void mmu_section_map(unsigned int *ttb, unsigned int va, unsigned int pa, enum CACHE_MODE mode)
{
    va /= 0x100000;

    ttb[va] = (pa & 0xFFF00000) | MMU_SECTION_AP | MMU_SECTION_DOMAIN | MMU_SECTION_FORCE_BIT | (mode << 2);
}


/*
    0x6000_0000~0xFFFF_FFFF Not Use
    0x4800_0000~0x6000_0000 SFR Area
    0x4000_0000~0x4000_0FFF BootSRAM(for nor flash)
    0x0000_0000~0x4000_0000 Bank0-Bank7

    VA                        PA                       Mode
    Bank0~Bank7:
    0~0x4000_0000             0~0x4000_0000            E_CACHE_WB
    Boot SRAM:
    0x4000_0000~0x4010_0000   0x4000_0000~0x4010_0000  E_CACHE_WB
    SFR:
    0x4800_0000~0x6000_0000   0x4800_0000~0x6000_0000  E_CACHE_NCNB
*/

void mmu_create_table(void)
{
    unsigned int *ttb = (unsigned int *)MMU_TTB_BASE;

    unsigned int va = 0;
    unsigned int pa = 0;

    /* Bank0~Bank7 */
    va = 0;
    pa = 0;
    for (;va < 0x40000000 && pa < 0x40000000;)
    {
        mmu_section_map(ttb, va, pa, E_CACHE_WB);
        va += 100000;
        pa += 100000;
    }

    /* Boot SRAM */
    va = 0x40000000;
    pa = 0x40000000;
    mmu_section_map(ttb, va, pa, E_CACHE_WB);

    /* SFR */
    va = 0x48000000;
    pa = 0x48000000;
    for (;va < 0x60000000 && pa < 0x60000000;)
    {
        mmu_section_map(ttb, va, pa, E_CACHE_NCNB);
        va += 100000;
        pa += 100000;
    }

    /* for sram/nor flash */
    mmu_section_map(ttb, 0, 0, E_CACHE_NCNB);

    /* for Framebuffer : 0x33b00000 */
    mmu_section_map(ttb, 0x33b00000, 0x33b00000, E_CACHE_NCNB);

}

