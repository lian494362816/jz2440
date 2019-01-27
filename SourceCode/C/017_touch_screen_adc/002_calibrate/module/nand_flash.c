#include "nand_flash.h"
#include "s3c2440_soc.h"


#define NAND_FLASH_DELAY_TIME (10)
#define NAND_FLASH_PAGE_SIZE (2048)
#define NAND_FLASH_BLOCK_SIZE (128 * 1024)

static void nand_flash_select(void)
{
    NFCONT &= ~(0x1 << 1);

}

static void nand_flash_deselect(void)
{
    NFCONT |= (0x1 << 1);
}

static void nand_flash_wait_ready(void)
{
    //[0], 0->busy, 1->ready

    while(!(NFSTAT & 0x1))
    {
        //nothing
    }
}

static void nand_flash_write_byte(unsigned char val)
{
    NFDATA = val;
}

static unsigned char nand_flash_data(void)
{
    return NFDATA;
}

void nand_flash_addr_byte(unsigned char addr)
{
    volatile int i = 0;
    NFADDR = addr;

    for (i = 0; i < NAND_FLASH_DELAY_TIME; i++);
}

void nand_flash_cmd(unsigned char cmd)
{
    volatile int i = 0;
    NFCMMD = cmd;

    for (i = 0; i < NAND_FLASH_DELAY_TIME; i++);
}

void nand_flash_init(void)
{
    NFCONF &= ~( (0x7 << 4) | (0x7 << 8) | (0x3 << 12) );

    //set TWRPH1, Duration = HCLK x (TWRPH1 + 1) >= 5
    NFCONF |= (0x0 << 4);

    //set TWRPH0, Duration = HCLK x ( TWRPH0 + 1 ) >=12
    NFCONF |= (0x1 << 8);

    //set TACLS, Duration = HCLK x TACLS >= 0
    NFCONF |= (0x0 << 12);


    //Disable Soft lock
    NFCONT &= ~(0x1 << 12);

    //Unlock spare area ECC
    //NFCONT &= ~(0x1 << 6);

    //Unlock Main area ECC
    //NFCONT &= ~(0x1 << 5);

    //init ECC
    NFCONT |= 0x1 <<4;

    //nand disable select
    NFCONT |= (0x1 << 1);

    //enable nand flash control
    NFCONT |= 0x1;

    //NFCONT = (1<<4) | (1<<1) | (1<<0);
}

/* nand address organize
1st Cycle A0 A1 A2 A3 A4 A5 A6 A7
2nd Cycle A8 A9 A10 A11 *L *L *L *L
3rd Cycle A12 A13 A14 A15 A16 A17 A18 A19
4th Cycle A20 A21 A22 A23 A24 A25 A26 A27
5th Cycle A28 *L *L *L *L *L *L *L
A0~A11 column addr
A13~A28 row   addr
*/

int nand_flash_read(unsigned int addr, char *buf, unsigned int size)
{
    unsigned int page_num = addr / NAND_FLASH_PAGE_SIZE;
    unsigned int column = addr & (NAND_FLASH_PAGE_SIZE - 1);
    unsigned int i = 0;

    nand_flash_select();

    while(i < size)
    {
        /* 1 send 0x00 cmd */
        nand_flash_cmd(0x00);

        /* 2 send 5 address */
        nand_flash_addr_byte(column & 0xFF);//A0~A7
        nand_flash_addr_byte((column >> 8)& 0xFF); //A8~A10, don't care A11
        nand_flash_addr_byte(page_num & 0xFF); //A12~A19
        nand_flash_addr_byte((page_num >> 8) & 0xFF);//A20~A27
        nand_flash_addr_byte((page_num >> 16) & 0xFF);//A28

        /* 3 send 0x30 cmd */
        nand_flash_cmd(0x30);

        /* 4 wait ready */
        nand_flash_wait_ready();

        /* 5 get data */
        while(column < NAND_FLASH_PAGE_SIZE && i < size)
        {
            buf[i] = nand_flash_data();
            i ++;
            column ++;
        }

        page_num ++;
        column = 0;
    }

    nand_flash_deselect();

    return 0;

}

int nand_flash_write(unsigned int addr, char *buf, unsigned int size)
{
    unsigned int i = 0;
    unsigned int page_num = addr / NAND_FLASH_PAGE_SIZE;
    unsigned int column = addr & (NAND_FLASH_PAGE_SIZE - 1);
    unsigned char status = 0;

    nand_flash_select();

    while(i < size)
    {
        //1 send 0x80 cmd
        nand_flash_cmd(0x80);

        //2 send 5 addr
        nand_flash_addr_byte(column & 0xFF);//A0~A7
        nand_flash_addr_byte((column >> 8)& 0x7); //A8~A10, don't care A11
        nand_flash_addr_byte(page_num & 0xFF); //A12~A19
        nand_flash_addr_byte((page_num >> 8) & 0xFF);//A20~A27
        nand_flash_addr_byte((page_num >> 16) & 0x1);//A28

        //3 send data
        while (i < size && column < NAND_FLASH_PAGE_SIZE)
        {
            nand_flash_write_byte(buf[i]);
            i++;
            column ++;
        }
        //4 send 0x10 cmd
        nand_flash_cmd(0x10);

        nand_flash_wait_ready();

        //5 send 0x70 cmd
        nand_flash_cmd(0x70);

        //6 check write OK?
        status = nand_flash_data();
        if (status & 0x1)
        {
            printf("nand write error\n");
            nand_flash_deselect();
            return -1;
        }

        column = 0;
        page_num ++;
    }

    nand_flash_deselect();

    return 0;
}

int nand_flash_erase(unsigned int addr, unsigned int size)
{
    unsigned int i = 0;
    unsigned int page_num = 0;
    int status = 0;

    if (addr & (NAND_FLASH_BLOCK_SIZE - 1))
    {
        printf("addr should 128K mul\n");
        return -1;
    }

    if (size & (NAND_FLASH_BLOCK_SIZE - 1))
    {
        printf("size should 128K mul\n");
        return -1;
    }

    nand_flash_select();

    while(i < size)
    {
        page_num = addr / NAND_FLASH_PAGE_SIZE;

        //1 send 0x60 cmd
        nand_flash_cmd(0x60);

        //2 send page addr
        nand_flash_addr_byte(page_num & 0xFF); //A12~A19
        nand_flash_addr_byte((page_num >> 8) & 0xFF);//A20~A27
        nand_flash_addr_byte((page_num >> 16) & 0x1);//A28

        //3 send 0xd0 cmd
        nand_flash_cmd(0xD0);

        nand_flash_wait_ready();

        //4 send 0x70 cmd
        nand_flash_cmd(0x70);

        status = nand_flash_data();
        if (status & 0x1)
        {
            printf("nand erase error\n");
            nand_flash_deselect();
            return -1;
        }

        i += NAND_FLASH_BLOCK_SIZE;
        page_num += NAND_FLASH_BLOCK_SIZE;
    }

    nand_flash_deselect();

    return 0;
}

static void nand_flash_id(void)
{
    int i = 0;
    unsigned char id_array[5];
    nand_flash_init();
    nand_flash_select();

    nand_flash_cmd(0x90);
    nand_flash_addr_byte(0x0);

    for (i = 0; i < 5; i++)
    {
        id_array[i] = nand_flash_data();
        printf("[%d] = 0x%x\n", i, id_array[i]);
    }

    //1st byte
    printf("Maker Code:0x%x\n", id_array[0]);
    //2st byte
    printf("Device Code:0x%x\n", id_array[1]);

    //3rd byte
    printf("Chip Num:%d \n", 0x1 << (id_array[2] & 0x3));

    if (0 == id_array[2] & (0x3 << 2))
    {
        printf("Level cell:%d \n", 2);
    }
    else
    {
        printf("Level cell:%d \n", 0x1 << ((id_array[2] & (0x3 << 2)) >> 2) );
    }

    printf("simultaneously program pages:%d \n", 0x1 << ((id_array[2] & (0x3 << 4)) >> 4) );

    if ((0x1 << 6) & id_array[2])
    {
        printf("Interleave Program Between multiple chips support\n");
    }

    if ((0x1 << 7) & id_array[2])
    {
        printf("Cache program support\n");
    }

    //4th
    printf("Page Size:%dKB \n", 0x1 << (id_array[3] & 0x3));

    printf("Block Size:%dKB \n", 64 << ((id_array[3] & (0x3 << 4)) >> 4) );

    if (id_array[3] & (0x1 << 6))
    {
        printf("Organization:16bit\n");
    }
    else
    {
        printf("Organization:8bit\n");
    }

    //5th
    printf("Plane Number:%d \n", 0x1 << ((id_array[4] & (0x3 << 2)) >> 2) );
    printf("Plane Size:%dMb \n", 64 << ((id_array[4] & (0x7 << 4)) >> 4) );

    nand_flash_deselect();
}

static void nand_flash_erase_test(void)
{
    unsigned int address = 0;
    int ret = 0;

    printf("pleae input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    ret = nand_flash_erase(address, NAND_FLASH_BLOCK_SIZE);

    if (ret != 0)
    {
        printf("erase fail\n");
    }
}


static void nand_flash_write_test(void)
{
    unsigned int address = 0;
    unsigned int val = 0;
    int i = 0;
    char string[100] = {0};
    int ret = 0;

    printf("please input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    printf("please input string:");
    gets(string);

    ret = nand_flash_write(address, string, strlen(string));
    if (0 != ret)
    {
        printf("write fail\n");
    }
}


static void nand_flash_read_test(void)
{
    unsigned int address = 0;
    char buff[64];
    int i = 0;
    int j = 0;
    int ret = 0;

    printf("please input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    ret = nand_flash_read(address, buff, 64);

    if (ret != 0)
    {
        printf("read fail\n");
    }

    printf("Data:\n");
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 16; j++)
        {
            printf("%02x ", buff[16 * i + j]);
        }

        printf("    ;");
        for (j = 0; j < 16; j++)
        {
            //Visual character in ascii
            if (buff[j] >= 32 && buff[j] <= 126)
            {
                printf("%c", buff[16 * i + j]);
            }
            else
            {
                printf(".");
            }
        }

        printf("\n");
    }

    printf("\n");

}


void nand_flash_test(void)
{
    char opt = 0;

     while(1)
     {
        printf("[g] Get   nand flash id \n\r");
        printf("[e] Erase nand flash \n\r");
        printf("[w] Write nand flash \n\r");
        printf("[r] Read  nand flash \n\r");
        printf("[q] Quit \n\r");
        printf("Enter selection: ");

        opt = getchar();
        printf("%c\n", opt);

        switch(opt)
        {
            case 'g':
            case 'G':
                nand_flash_id();
                break;

            case 'e':
            case 'E':
                nand_flash_erase_test();
                break;

            case 'w':
            case 'W':
                nand_flash_write_test();
                break;

            case 'r':
            case 'R':
                nand_flash_read_test();
                break;

            case 'q':
            case 'Q':
                return;

            default:
                break;
        }
    }
}

