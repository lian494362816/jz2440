
.global _start
_start:

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

    /* stop watch dog */
    ldr r0, =0x53000000
    mov r1, #0
    str r1, [r0]

    bl main

loop:
	b loop
