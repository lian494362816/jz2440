
.text

.global _start
_start:


/* 1, set output mode 
 *  GPFCON [9:8] 01
 *  0x56000050
 */
	ldr r1, =0x56000050
	ldr r0, =0x100
	str r0, [r1]

/* 2, set value to 0 
 * GPFDAT [4] 0
 * 0x56000054
 */
   ldr r1, =0x56000054
   ldr r0, =0
   str r0, [r1]
 
 loop:
	b loop
    
