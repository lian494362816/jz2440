
#include "s3c2440_soc.h"
#include "setup.h"

#define KERNEL_SIZE (2 * 1024 * 1024)
/* uImage = header + image, header=64byte */
#define KERNEL_SRC_ADDR (0x60000 +64)
#define KERNEL_DST_ADDR (0x30008000)
#define SDRAM_START_ADDR (0x30000000)
#define SDRAM_SIZE (64 * 1024 * 1024)
#define COMMAND ("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0,115200")

#define MACH_TYPE_S3C2440              362
#define BOOT_PARAMS (0x30000100)

static struct tag *params;


void uart_init(void)
{
    /* 1 set gpio to uart mode
        Tx GPH2 [5:4] b10
        Rx GPH3 [7:6] b10
    */
    GPHCON &= ~((0x3 << 4) | (0x3 << 6));
    GPHCON |= (0x2 << 4) | (0x2 << 6);

    /* 2 set pull up */
    GPHUP |= (0x1 << 2) | (0x1 << 3);

    /* 3 set formar 8N1 */
    ULCON0 |= 0x3;

    /* 4 set Soruce clock PCLK and poling mode*/
    UCON0 = 0x5;

    /* 5 set baud rate 115200
    PCLK = 50M
    UBRDIVn   = (int)( UART clock / ( buad rate x 16) ) ¨C1
    UBRDIVn = (50M / (115200 x 16))-1 = 26.1267 -> 26
    */
    UBRDIV0 = 26;
}

int getchar(void)
{
    int chr = 0;

    while (!(UTRSTAT0 & 0x1))
    {
        //nothing
    }

    chr = URXH0;

    return chr;
}

int putchar(int c)
{
    while (!(UTRSTAT0 & 0x4))
    {
        //nothing
    }

    UTXH0 = (unsigned char )c;
}


int puts(const char *s)
{
    while(*s)
    {
        putchar(*s++);
    }
}

void puthex(unsigned int hex)
{
    int i = 0;
    unsigned int tmp = 0;

    puts("0x");

    for (i = 7; i >= 0; i--)
    {
        tmp = (hex >> (4*i)) & 0xF;
        if (tmp >= 0 && tmp <= 9)
        {
            putchar(tmp + '0');
        }
        else
        {
            putchar(tmp - 10 + 'A');
        }
    }

    puts("\n\r");

}

void put_byte_hex(unsigned int hex)
{
    int i = 0;
    unsigned int tmp = 0;

    for (i = 1; i >= 0; i--)
    {
        tmp = (hex >> (4*i)) & 0xF;
        if (tmp >= 0 && tmp <= 9)
        {
            putchar(tmp + '0');
        }
        else
        {
            putchar(tmp - 10 + 'A');
        }
    }

}


int led_init(void)
{
    /* clear mode */
    GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    /* set to output mode */
    GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
    /* all led off */
    GPFDAT |= (7 << 4);

    return 0;
}

int led_on(int led_num)
{
    if (4 == led_num)
    {
        GPFDAT &= ~(1 << 4);
    }
    else if (5 == led_num)
    {
        GPFDAT &= ~(1 << 5);
    }
    else if (6 == led_num)
    {
        GPFDAT &= ~(1 << 6);
    }
    else
    {
        return -1;
    }

    return 0;
}

char * strcpy(char * dest,const char *src)
{
    char *tmp = dest;

    while ((*dest++ = *src++) != '\0')
        /* nothing */;
    return tmp;
}

int strlen(const char * s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

static void setup_start_tag (void)
{
    params = (struct tag *) BOOT_PARAMS;

    params->hdr.tag = ATAG_CORE;
    params->hdr.size = tag_size (tag_core);

    params->u.core.flags = 0;
    params->u.core.pagesize = 0;
    params->u.core.rootdev = 0;

    params = tag_next (params);
}

static void setup_memory_tags (void)
{
    params->hdr.tag = ATAG_MEM;
    params->hdr.size = tag_size (tag_mem32);

    params->u.mem.start = SDRAM_START_ADDR;
    params->u.mem.size = SDRAM_SIZE;

    params = tag_next (params);
}

static void setup_commandline_tag (char *commandline)
{
    int len = strlen(commandline) + 1;

    params->hdr.tag = ATAG_CMDLINE;
    params->hdr.size = (sizeof (struct tag_header) + len + 3) >> 2;

    strcpy (params->u.cmdline.cmdline, commandline);

    params = tag_next (params);
}

static void setup_end_tag (void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

int main(void)
{
    int i = 0;
    void (*theKernel)(int zero, int arch, unsigned int params);

    /* into kernel, must enable uart */
    uart_init();
    puts("uart init ok \n\r");

    puthex(*(unsigned int *)KERNEL_DST_ADDR);
    /*copy kernel to sdram */
    nand_flash_read(KERNEL_SRC_ADDR, (char *)KERNEL_DST_ADDR, KERNEL_SIZE);


    setup_start_tag();
    setup_memory_tags();
    setup_commandline_tag(COMMAND);
    setup_end_tag();
    puts("set tag ok \n\r");

    puthex(*(unsigned int *)KERNEL_DST_ADDR);

    theKernel = (void (*)(int,int, unsigned int))KERNEL_DST_ADDR;
    theKernel (0, MACH_TYPE_S3C2440, BOOT_PARAMS);
    puts("jump to kernel error \n\r");

    return -1;
}
