//  �� �� ��   : oled.c
//  �� �� ��   : v1.0
//  ��    ��   : fyywhy
//  ��д����   : 2020-02-22
//  ��������   : 0.96��OLED��������

/*
*----------------------------------------------------------------
*	GND   ��Դ��
*	VCC   ��5V��3.3v��Դ
*	SCL   ��PB8��SCL��
*	SDA   ��PB9��SDA��            
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

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

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
	 

