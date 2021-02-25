//  文 件 名   : oled.c
//  版 本 号   : v1.0
//  作    者   : fyywhy
//  编写日期   : 2020-02-22
//  功能描述   : 0.96寸OLED驱动程序

/*
*----------------------------------------------------------------
*	GND   电源地
*	VCC   接5V或3.3v电源
*	SCL   接PB8（SCL）
*	SDA   接PB9（SDA）            
* ----------------------------------------------------------------
*/

#include "i2c.h"

#ifndef __DRV_OLED_H

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void oled_init(void);
void oled_clear(void);
void oled_on(void);
void oled_display_on(void);
void oled_display_off(void);

void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t char_size);
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);
void oled_show_int_number(uint8_t x,uint8_t y,int num,uint8_t char_size);
void oled_show_float_number(uint8_t x,uint8_t y,float num,uint8_t char_size);
void oled_show_string(uint8_t x,uint8_t y,uint8_t *chr,uint8_t char_size);
void oled_show_chinese(uint8_t x,uint8_t y,uint8_t no);


#endif  
	 

