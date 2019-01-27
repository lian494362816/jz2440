
.global _start
_start:

	ldr sp, =4096 /* for nand */
	/*ldr sp, =0x40000000 + 4096  /* for nor */
	bl main

loop:
	b loop
