
#include "mcu_info.h"
#include "gd32f10x.h"
#include <stdio.h>

static uint32_t int_device_serial[3];
static uint32_t chip_ram_size;

static void get_chip_ram_size(void);
static void get_chip_serial_num(void);
static void print_mcu_id(void);
static void print_mcu_rom_size(void);


void print_mcu_info(void)
{
	print_mcu_id();
	print_mcu_rom_size();
}

static void get_chip_ram_size(void)
{
	chip_ram_size = *(__IO uint16_t*)(0x1FFFF7E0);
}

static void get_chip_serial_num(void)
{
    int_device_serial[0] = *(__IO uint32_t*)(0x1FFFF7E0);
    int_device_serial[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    int_device_serial[2] = *(__IO uint32_t*)(0x1FFFF7F0);
}

static void print_mcu_id(void)
{
	get_chip_serial_num();
    printf("\n\r The CPU Unique Device ID:[%X-%X-%X]\n\r", 
		int_device_serial[2], 
		int_device_serial[1],
		int_device_serial[0]
	);
}

static void print_mcu_rom_size(void)
{
	get_chip_ram_size();
	printf("\n\r The CPU Flash size:%dK\n\r",chip_ram_size);
}

