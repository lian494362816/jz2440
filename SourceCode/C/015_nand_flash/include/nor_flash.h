#ifndef __NOR_FLASH_H__
#define __NOR_FLASH_H__

void nor_flash_cmd(unsigned int address, unsigned int val);
unsigned int nor_flash_data(unsigned int address);

void nor_flash_test(void);
void nor_flash_scan(void);

int nor_flash_write(unsigned int address, char *buff, unsigned int size);
int nor_flash_read(unsigned int address, char *buff, unsigned int size);
int nor_flash_erase(unsigned int address);

#endif
