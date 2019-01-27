
SECTIONS {
    . = 0x33f80000 ;
    
    . = ALIGN(4) ;
    __copy_code_start = . ;
    .init :
    {
        ./start.o
    }

    .text :
    {
        *(.text)
    }


    . = ALIGN(4) ;
    .rodata :
    {
        *(.rodata)
    }

    . = ALIGN(4) ;
    .data :
    {
        *(.data)
    }

    . = ALIGN(4) ;
    __bss_start__ = .;
    .bss :
    {
        *(.bss)
        *(.COMMON)
    }
    __end__ = . ;

}
