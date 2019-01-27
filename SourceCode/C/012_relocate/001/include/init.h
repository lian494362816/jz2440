#ifndef __INIT_H__
#define __INIT_H__

/*
000 = 1 clock        001 = 2 clocks
010 = 3 clocks       011 = 4 clocks
100 = 6 clocks       101 = 8 clocks
110 = 10 clocks      111 = 14 clocks
*/
int bank0_tacc_set(int value);

//bank6 start address 0x30000000, DRAM 64MB
int bank6_sdram_init(void);


#endif //__INIT_H__
