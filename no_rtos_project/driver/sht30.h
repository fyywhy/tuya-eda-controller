#ifndef __SHT31_H
#define __SHT31_H

#include "gd32f10x.h"

void sht30_init(void);
uint8_t sht30_read_serial_number(uint32_t* sn);
uint8_t sht30_read_singleshot(float *temp_val,float *Humi_val);	/* ���λ�ȡ */
uint8_t sht30_read_periodic(float *temp_val,float *Humi_val); /* ���ڻ�ȡ */

#endif

