#include "s3c2440_soc.h"
#include "init.h"


int bank0_tacc_set(int value)
{

    /* nor flash can change 0->1, but can't change 1->0
    so can't clear [10:8]
    */
    //BANKCON0 &= ~(0x7 << 8);
    //BANKCON0 |= (value << 8);
    BANKCON0 = value << 8;

    return 0;
}

int bank6_sdram_init(void)
{

    /*1, set BWSCON */
    //clear bank6 [24:27]
    BWSCON &= ~(0xF << 24);

    //set data bus [25:24] 00->8bit, 01->16bit, 10->32bit
    BWSCON |= 2 << 24;

    //[26]=0, disable WAIT
    BWSCON |= 0 << 26;

    //[27]=0, Not useing UB/LB, it means use nWBE
    BWSCON |= 0 << 27;

    /*2, set BANKCON6 */
    //clear [0:3]
    BANKCON6 &= ~(0xF);

    //[1:0] SCAN(Column Num) 00->8bit, 01->9it, 10->10bit
    BANKCON6 |= 1 << 0;

    //[3:2] = 0, Trcd(RAS to CAS delay) 00->2 clock, 01->3clock, 10->4clock
    BANKCON6 |= 0 << 2;

    //clear [16:15]
    //00->ROM or SRAM, 11->SDRAM
    BANKCON6 |= 3 << 15;

    /*3, set REFRESH */
    //clear [0:23]
    REFRESH &= ~(0xFFFFFF);

    //Refresh counter, refresh time = 7.8us,
    //counter = 2^11 +1 - 100*7.8 = 1269= 0x45F
    REFRESH |= 0x4F5;

    //[19:18]=00, Tsrc 00->4clock, 01->5clock, 10->6clock, 11->7clock
    //Trc=Tsrc+Trp, Trc=2clock, Trc=60ns=6clock, Tsrc=6-2=4clock
    REFRESH |= 0 << 18;

    //[21:20] Trp(pre-charge), 00->2clock, 01->3clock, 10->4clock
    REFRESH |= 0 << 20;

    //[22] TREFMD 0->Auto Refresh, 1->Self Refresh
    REFRESH |= 0 << 22;

    //[23] REFEN 0->Disable, 1->Enable
    REFRESH |= 1 << 23;

    /*4, set BANKSIZE */
    //clear [7:0]
    BANKSIZE &= ~(0xFF);

    /*[2:0] memery map
    010->128M,
    001->64M,
    000->32M
    111->16M
    110->8M
    101->4M
    100->4M
    */
    BANKSIZE |= 1 << 0;

    //[4] SLCK_EN mode 0->always, 1 active druing access
    BANKSIZE |= 1 << 4;

    //[5] SCKE_EN, power donw mode,0->disable, 1->enable
    BANKSIZE |= 1 << 5;

    //[7] burst enable, 0->disable, 1->enable
    BANKSIZE |= 1 << 7;

    /*5 set MRSRB6 */
    //clear [9:0]
    MRSRB6 &= ~(0x3FF);

    //[2:0] brust length, must be 0, burst length = 1
    MRSRB6 |= 0 << 0;

    //[3] Burst byte, must be 0, sequential
    MRSRB6 |= 0 << 3;

    //[6:4] CAS latency, 000->1clock, 010->2clock, 011->3clock
    //can be 2clock or 3clock
    MRSRB6 |= 2 << 4;

    //[8:7] Test Mode, must be 0
    MRSRB6 |= 0 << 7,

    //[9] Write burst length, 0->burst, must be 0
    MRSRB6 |= 0 << 9;

    return 0;
}

/* copy.text .data .rodata to SDRAM */
void copy2sdram(void)
{
    extern int __copy_code_start, __bss_start;

    volatile unsigned int *src = (unsigned int *)0;
    volatile unsigned int *dst = (unsigned int *)&__copy_code_start;
    volatile unsigned int *end = (unsigned int *)&__bss_start;

    while(dst < end)
    {
        *dst++ = *src++;
    }
}

void clear_bss(void)
{
    extern int __bss_start, __end;
    volatile unsigned int *src = (unsigned int *)&__bss_start;
    volatile unsigned int *end = (unsigned int *)&__end;

    while(src <= end)
    {
        *src ++ = 0;
    }
}

void print_hello(void)
{
    printf("\n");
    printf("hello\n");
}

void printf_undef(unsigned int cpsr_status, char *string)
{
    printf("%s\n", string);
    printf("cpsr:0x%x\n", cpsr_status);
}

