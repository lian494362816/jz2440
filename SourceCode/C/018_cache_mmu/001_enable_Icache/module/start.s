
.global _start
_start:

    b _reset              /* 0x00 reset */
    ldr pc, _undef_addr   /* 0x04 Undefined instruction */
    ldr pc, _swi_addr     /* 0x08 Software interrupt */
    bl  loop              /* 0x0c Abort (prefetch) */
    bl  loop              /* 0x10 Abort (data)     */
    bl  loop              /* 0x14 Reserved */
    ldr pc, _irq_addr     /* 0x18 IRQ */
    bl  loop              /* 0x1c FIQ */

_undef_addr: .word _undef
_swi_addr:     .word _swi
_irq_addr: .word _irq

_undef:

    /*set  undef_sp, sdram end address is 0x34000000
    because sdram size is 64M,sdram start address is 0x30000000 */
    ldr sp, =0x34000000

    /* store register */
    stmdb sp!, {r0-r12, lr}

    mrs r0, cpsr
    ldr r1, =undef_string
    bl print_excption

    /* resume register */
    ldmia sp!, {r0-r12, pc}^ /*^ means copy spsr to cpsr */

undef_string:
    .string "Undefine Excption!"
    .align 4

_swi:

    ldr sp, =0x33e00000

    stmdb sp!, {r0-r12, lr}

    /* r4 will not change when call a func */
    mov r4, lr

    mrs r0, cpsr
    ldr r1, =swi_string
    bl print_excption

    /* the lr is next address(PC + 4), get swi address should sub 4 */
    sub r0, r4, #4
    bl print_swi

    ldmia sp!, {r0-r12, pc}^

swi_string:
    .string "SWI Excption!"
    .align 4

_irq:
     ldr sp, =0x33c00000

    /* In IRQ, the return commond is:SUBS PC, R14_irq, #4
    so, sub R14_irq before stored*/
    sub lr, lr, #4
    stmdb sp!, {r0-r12, lr}

    /*
    mrs r0, cpsr
    ldr r1, =swi_string
    bl print_excption
    */
    bl irq_handle_c

    ldmia sp!, {r0-r12, pc}^

_reset:
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

    bl enable_icache

    bl bank6_sdram_init

    /* copy .data .rodata. .text to SDRAM */
    bl copy2sdram

    /* clear bss segment */
    bl clear_bss

    /* set usr_sp */
    ldr sp, =0x33f00000

    /*clear CPSR T(bit 7), so IRQ can trigger, the option can't int usr mode*/
    mrs r0, cpsr
    bic r0, r0, #(0x1 << 7)
    msr cpsr, r0

    /*after reset, the cpsr is in svc mode, should change to usr mod */
    mrs r0, cpsr
    bic r0, r0, #0xf
    msr cpsr, r0

    /* the nand flash is not init, should use nor flash */
    ldr pc, =sdram_addr

sdram_addr:

    bl uart0_init

    /* is an undefine instrucion */
    /* .word 0xeeadc0de */

    /*software interrupt */
    /* swi 0x111 */

    ldr pc, =main

loop:
    b loop


enable_icache:
    mrc p15, 0, r0, c1, c0, 0;
    orr r0, r0, #(0x1 << 12)
    mcr p15, 0, r0, c1, c0, 0;

    mov pc, lr
