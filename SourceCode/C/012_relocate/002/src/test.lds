
SECTIONS {
	.text   0  : { *(.text) }
	.rodata  : { *(.rodata) }
	.data 0x30000000 : AT(0xeff) { *(.data) }
	.bss  : { *(.bss) *(.COMMON) }
}
