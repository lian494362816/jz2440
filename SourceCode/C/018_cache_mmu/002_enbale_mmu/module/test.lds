
SECTIONS {
    . = 0x30000000 ;

    . = ALIGN(4) ;
    __copy_code_start = . ;
    .init :
    {
       ./module/start.o
       ./module/init/init.o
       ./module/mmu/mmu.o
       ./module/nand/nand_flash.o
    }

    .text :
    {
        *(.text)
    }

    . = ALIGN(4) ;
    .rodata : { *(.rodata) }

    . = ALIGN(4) ;
    .data : { *(.data) }

    . = ALIGN(4) ;
    __bss_start = . ;
    .bss : { *(.bss) *(.COMMON) }
    __end = . ;

}
