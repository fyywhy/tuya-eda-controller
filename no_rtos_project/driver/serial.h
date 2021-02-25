#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "gd32f10x.h"
#include <stdio.h>

void serial_init(void);

void uart1_send_data(unsigned char value);
#endif





