#include "nor_flash.h"
#include "my_printf.h"

/* nor flash use cs0, the cs0 start address is 0x00000000 */
#define NOR_FLASH_BASE (0x0)
#define NOR_FLAHS_SIZE (1024 * 1024 *2) //2M byte
#define NOR_FLASH_ADDRESS_START (NOR_FLASH_BASE + 0x0)
#define NOR_FLASH_ADDRESS_END (NOR_FLASH_BASE + NOR_FLAHS_SIZE) //2M byte

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

int nor_flash_write(unsigned int address, char *buff, unsigned int size)
{
    int val = 0;
    int two_byte_align = 0;
    int i = 0;

    if (size < 0)
    {
        return 0;
    }

    if (NULL == buff)
    {
        printf("nor flash write NULL pointer \n");
        return -1;
    }

    if (address < NOR_FLASH_ADDRESS_START | (address + size) > NOR_FLASH_ADDRESS_END)
    {
        printf("nor flash addres range:%08x~%08x\n", NOR_FLASH_ADDRESS_START, NOR_FLASH_ADDRESS_END);
        printf("write range:%08x~%08x\n", address, address + size);
        return -1;
    }

    nor_flash_reset_mode();

    printf("address:0x%x\n", address);
    printf("size:%d\n", size);

    // 2 byte aligment
    if (0 == (size % 2))
    {
        two_byte_align = 1;
    }
    else
    {
        two_byte_align = 0;
        size -= 1;
    }

    while(i < size)
    {
        //unlock
        nor_flash_cmd(0x555, 0xAA);
        nor_flash_cmd(0x2AA, 0x55);
        //into program mode
        nor_flash_cmd(0x555, 0xA0);

        val = buff[i] + (buff[i + 1] << 8);
        nor_flash_cmd(address >> 1, val);

        i += 2;
        address += 2;
    }

    if (!two_byte_align)
    {
        //unlock
        nor_flash_cmd(0x555, 0xAA);
        nor_flash_cmd(0x2AA, 0x55);
        //into program mode
        nor_flash_cmd(0x555, 0xA0);

        val = buff[i];
        nor_flash_cmd(address >> 1, val);
    }

    nor_flash_wait_ready(address);

    nor_flash_reset_mode();

    return 0;
}

int nor_flash_read(unsigned int address, char *buff, unsigned int size)
{
    int i = 0;
    volatile unsigned char *p = (volatile unsigned char *)address;

    if (0 == size)
    {
        return 0;
    }

    if (NULL == buff)
    {
        printf("nor flash read NULL pointer\n");
        return -1;
    }

    if (address < NOR_FLASH_ADDRESS_START | (address + size) > NOR_FLASH_ADDRESS_END)
    {
        printf("nor flash addres range:%08x~%08x\n", NOR_FLASH_ADDRESS_START, NOR_FLASH_ADDRESS_END);
        printf("read range:%08x~%08x\n", address, address + size);
        return -1;
    }

    nor_flash_reset_mode();

    while (i < size)
    {
        buff[i] = *p;
        p ++;
        i ++;
    }

    return 0;
}

int nor_flash_erase(unsigned int address)
{
    if (address < NOR_FLASH_ADDRESS_START | address  > NOR_FLASH_ADDRESS_END)
    {
        printf("nor flash addres range:%08x~%08x\n", NOR_FLASH_ADDRESS_START, NOR_FLASH_ADDRESS_END);
        printf("erase address:%08x\n", address);
        return -1;
    }

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

    return 0;
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

static void nor_flash_erase_test(void)
{
    unsigned int address = 0;
    int ret = 0;

    printf("pleae input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    ret = nor_flash_erase(address);

    if (ret != 0)
    {
        printf("erase fail\n");
    }
}

static void nor_flash_read_test(void)
{
    unsigned int address = 0;
    char buff[100];
    int i = 0;
    int j = 0;
    int ret = 0;

    printf("please input address:");
    address = get_uint();
    printf("address:0x%08x\n", address);

    ret = nor_flash_read(address, buff, 64);

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

static void nor_flash_write_test(void)
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

    ret = nor_flash_write(address, string, strlen(string));
    if (0 != ret)
    {
        printf("write fail\n");
    }
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
                nor_flash_erase_test();
                break;

            case 'w':
            case 'W':
                nor_flash_write_test();
                break;

            case 'r':
            case 'R':
                nor_flash_read_test();
                break;

            case 'q':
            case 'Q':
                return;

            default:
                break;
        }
    }
}


