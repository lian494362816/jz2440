#include "nor_flash.h"
#include "my_printf.h"

/* nor flash use cs0, the cs0 start address is 0x00000000 */
#define NOR_FLASH_BASE (0x0)

/* nor flash AO~A19 is connet with LADDR1~LADDR20, so the LADDR0 is not used
The real address needs to be moved one bit to the left*/
#define TO_NOR_FLASH_ADDRESS(addr) ((addr) << 0x1)

#define NULL ((void *)0)

static void nor_flash_write_word(unsigned int base, unsigned int address, unsigned int val)
{
    volatile unsigned short *pa = (volatile unsigned short *)(base + TO_NOR_FLASH_ADDRESS(address));
    *pa = val;
}

static unsigned int nor_flash_read_word(unsigned int base, unsigned int address)
{
    volatile unsigned short *pa = (volatile unsigned short *)(base + TO_NOR_FLASH_ADDRESS(address));
    return *pa;
}

void nor_flash_cmd(unsigned int address, unsigned int val)
{
    nor_flash_write_word(NOR_FLASH_BASE, address, val);
}

unsigned int nor_flash_data(unsigned int address)
{
    return nor_flash_read_word(NOR_FLASH_BASE, address);
}

static void nor_flash_reset_mode(void)
{
    //reset mode, any address write 0xF0
    nor_flash_cmd(0xFF, 0xF0);
}

static void nor_flash_wait_ready(unsigned int address)
{
    int old = 0;
    int cur = 0;

    //when flash is in erasing/progrming, the Q6 will toggle

    old = nor_flash_data(address) & (0x1 << 6);
    delay(1);
    cur = nor_flash_data(address) & (0x1 << 6);

    while(old != cur)
    {
        old = nor_flash_data(address) & (0x1 << 6);
        delay(1);
        cur = nor_flash_data(address) & (0x1 << 6);
    }
}

void nor_flash_scan(void)
{
    char qry[4] = {0};
    unsigned int size = 0;
    unsigned int boot_num = 0;
    unsigned int erase_region_cnt = 0;
    unsigned int erase_region_num = 0;
    unsigned int erase_block_cnt = 0;
    unsigned int erase_block_size = 0;
    unsigned int erase_block_address = 0;
    unsigned int erase_region_bank_address = 0x2D;
    unsigned int manifacture_id = 0;
    unsigned int device_id = 0;
    int i = 0;
    int j = 0;
    int tmp_cnt = 0;

    //unlock
    nor_flash_cmd(0x555, 0xAA);
    nor_flash_cmd(0x2AA, 0x55);
    nor_flash_cmd(0x555, 0x90);

    //read manifacure id
    manifacture_id = nor_flash_data(0x0);
    //read device id
    device_id = nor_flash_data(0x1);

    printf("manifacture id:0x%x\n", manifacture_id);
    printf("device id:0x%x\n", device_id);

    nor_flash_reset_mode();

    //enter cfi mode
    nor_flash_cmd(0x55, 0x98);

    //get "QRY" string
    qry[0] = nor_flash_data(0x10);
    qry[1] = nor_flash_data(0x11);
    qry[2] = nor_flash_data(0x12);
    qry[4] = '\0';

    //printf("%s \n", qry);

    //get Device size, Device size = 2^n in number of bytes
    size = ( 0x1 << nor_flash_data(0x27) );
    printf("Size:0x%x, %dM\n", size, size / 1024 / 1024);

    // Top/Bottom boot block indicator
    // 02h=bottom boot device 03h=top boot device
    boot_num = nor_flash_data(0x4F);
    if (0x2 == boot_num)
    {
        printf("bottom boot device\n");
    }
    else if (0x3 == boot_num)
    {
        printf("top boot device\n");
    }
    else
    {
        printf("not find boot devide:%d", boot_num);
    }

    // erase regions count
    erase_region_cnt = nor_flash_data(0x2C);
    printf("erase region count:%d\n", erase_region_cnt);

    /*
    bits 31- 16 = z,  where the Erase Block(s) within this
    Region are (z) times 256 bytes
    bits 15 - 0 = y, where y+1 = Number of Erase Blocks
    of identical size within region
    */
    for (i = 0; i < erase_region_cnt; i++)
    {
        erase_block_cnt = 1 + nor_flash_data(erase_region_bank_address + 0) + (nor_flash_data(erase_region_bank_address + 1) << 8);
        erase_block_size = 256 * ( nor_flash_data(erase_region_bank_address + 2) + (nor_flash_data(erase_region_bank_address + 3) << 8) );
        printf("region[%d], block cnt:%d, block size:0x%x \n", i + 1, erase_block_cnt, erase_block_size);
        erase_region_bank_address += 4;
    }

    //every block address
    printf("Sector Start Address:\n");
    erase_region_bank_address = 0x2D;
    for (i = 0; i < erase_region_cnt; i++)
    {
        erase_block_cnt = 1 + nor_flash_data(erase_region_bank_address + 0) + (nor_flash_data(erase_region_bank_address + 1) << 8);
        erase_block_size = 256 * ( nor_flash_data(erase_region_bank_address + 2) + (nor_flash_data(erase_region_bank_address + 3) << 8) );

        for (j = 0; j < erase_block_cnt; j++)
        {
            printf("0x%08x ", erase_block_address);
            tmp_cnt ++;

            erase_block_address += erase_block_size;
            if (0 == (tmp_cnt % 5) )
            {
                printf("\n");
            }
        }

        erase_region_bank_address += 4;
    }

    //exit cif mode & reset flash
    nor_flash_reset_mode();
}

static void nor_flash_erase(void)
{
    unsigned int address = 0;

    printf("pleae input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    //unlock
    nor_flash_cmd(0x555, 0xAA);
    nor_flash_cmd(0x2AA, 0x55);

    //into Sector Erase mode
    nor_flash_cmd(0x555, 0x80);

    //unlock
    nor_flash_cmd(0x555, 0xAA);
    nor_flash_cmd(0x2AA, 0x55);

    //set Sector address
    /* nor_flash_cmd will move address one bit to left,
    if want to access the correct address, should move one bit to right*/
    nor_flash_cmd(address >> 1, 0x30);

    nor_flash_wait_ready(address >> 1);

    nor_flash_reset_mode();
}

static void nor_flash_read(void)
{
    unsigned int address = 0;
    volatile unsigned char *p = NULL;
    char tmp_buff[16] = {0};
    int i = 0;
    int j = 0;
    int cnt = 0;

    printf("please input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);
    p = (volatile unsigned char *)address;

    printf("Data:\n");
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 16; j++)
        {
            printf("%02x ", *p);
            tmp_buff[j] = *p;
            p++;
        }

        printf("    ;");
        for (j = 0; j < 16; j++)
        {
            //Visual character in ascii
            if (tmp_buff[j] >= 32 && tmp_buff[j] <= 126)
            {
                printf("%c", tmp_buff[j]);
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

static void nor_flash_write(void)
{
    unsigned int address = 0;
    unsigned int val = 0;
    int i = 0;
    char string[100] = {0};

    printf("please input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    printf("please input string:");
    gets(string);

    while(string[i] && string[i + 1])
    {
         //unlock
        nor_flash_cmd(0x555, 0xAA);
        nor_flash_cmd(0x2AA, 0x55);
        //into program mode
        nor_flash_cmd(0x555, 0xA0);

        val = string[i] + (string[i + 1] << 8);
        nor_flash_cmd(address >> 1, val);
        address += 2;
        i += 2;
    }

    /*
    case 1 string[i] != 0, string[i + 1] = 0
        val = string[i] + (0 << 8)
     case 2 string[i] == 0, string[i + 1] == 0
        val = 0
     case 3 string[i] == 0, string[i + 1] != 0
        val = 0, don't case string[i + 1]
     ====> shoule write string[i] in the end
    */

     //unlock
    nor_flash_cmd(0x555, 0xAA);
    nor_flash_cmd(0x2AA, 0x55);
    //into program mode
    nor_flash_cmd(0x555, 0xA0);

    val = string[i];
    nor_flash_cmd(address >> 1, val);

    nor_flash_wait_ready(address >> 1);

    nor_flash_reset_mode();
}

void nor_flash_test(void)
{
    char opt = 0;

     while(1)
     {
        printf("[s] Scan nor flash\n\r");
        printf("[e] Erase nor flash\n\r");
        printf("[w] Write nor flash\n\r");
        printf("[r] Read nor flash\n\r");
        printf("[q] quit\n\r");
        printf("Enter selection: ");

        opt = getchar();
        printf("%c\n", opt);

        switch(opt)
        {
            case 's':
            case 'S':
                nor_flash_scan();
                break;

            case 'e':
            case 'E':
                nor_flash_erase();
                break;

            case 'w':
            case 'W':
                nor_flash_write();
                break;

            case 'r':
            case 'R':
                nor_flash_read();
                break;

            case 'q':
            case 'Q':
                return;

            default:
                break;
        }
    }
}


