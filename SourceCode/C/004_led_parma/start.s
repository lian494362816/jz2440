
.global _start
_start:

	ldr sp, =4096 /* for nand */
	/*ldr sp, =0x40000000 + 4096  /* for nor */

    mov r0, #4
	bl led_on
	ldr r0,=10000
	bl delay
	mov r0, #5
	bl led_on

loop:
	b loop
