
.global _start
_start:

    /* stop watch dog */
    ldr r0, =0x53000000
    mov r1, #0
    str r1, [r0]

    /* Source  crystal oscillator 12M
    FCLK 400M HCLK 100M PCLK 50M */

    /* 1, set lock time LOCKTIME 0x4c000000 */
    ldr r0, =0x4c000000
    ldr r1, = 0xFFFFFFFF
    str r1, [r0]
    /*2, set CLKDIVN 0x4c000014
        HDIVN = FLCK / 4
        PDIVN = HCLK / 2
        HDIVN->b10 PDIVN->1 */
    ldr r0, = 0x4c000014
    ldr r1, = ( (2 << 1) | (1 << 0) )
    str r1, [r0]

    /* HCLK = FCLK/4 when CAMDIVN[9] = 0
    and thc CAMDIVN[9] default value is 0*/

    /*3, if HDIVN is not 0, CPU bus mode must be asynchronous */
    mrc  p15,0,r0,c1,c0,0
    orr  r0,r0, #0xc0000000
    mcr  p15,0,r0,c1,c0,0

    /* 4, set MPLLCON 0x4c000004
    MDIV->92(0x5c), PDIV->1 SDIV->1
    MPLL = (2 * m *Fin) / (p * 2 ^s)
    m = (MVID + 8) = (92 + 8) = 100
    p = (PDIV + 2) = (1 + 2) = 3
    s= SDIV = 1
    MPLL = (2 * 100 * 12M) / (3 * 2 ^ 1) = 400M
    */
    ldr r0, =0x4c000004
    ldr r1, =( (92 << 12) | (1 << 4) | (1 << 0))
    str r1, [r0]

    /* aoto switch nand or nor flash */
    /* write 0 to address[0] and read it
    * if address[0] is equal 0, it is nand
    * because nand can wirte easy, but nor need send
    * write-commond before wirte data, so write 0 to
    * nor address[0] will fail, and get nor address[0]
    * data is not equal 0
    */
    mov r1, #0
    ldr r0, [r1] /* r0=[0] store address[0] data*/
    str r1, [r1] /* 0->[0] */
    ldr r2, [r1] /* r2=[0] */
    cmp r1, r2
    ldr sp, =0x40000000 + 4096/* for nor */
    ldreq sp, =4096  /* for nand */
    streq r0, [r1] /* recover address[0] data */

    bl bank6_sdram_init

    /* copy .data .rodata. .text to SDRAM */
    bl copy2sdram

    /* clear bss segment */
    bl clear_bss

	bl uart0_init

	bl print_positon

    ldr pc, =main /* abs jump to main */

loop:
    b loop
