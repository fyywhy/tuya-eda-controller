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

#ifndef __OLED_H
#include "stdlib.h"
#include "gd32f10x.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

//-----------------OLED IIC�˿ڶ���----------------   
#define oled_sck_reset() 	gpio_bit_reset(GPIOB, GPIO_PIN_8) //SCL
#define oled_sck_set() 		gpio_bit_set(GPIOB, GPIO_PIN_8)

#define oled_sda_reset() 	gpio_bit_reset(GPIOB, GPIO_PIN_9) //SDA
#define oled_sda_set() 		gpio_bit_set(GPIOB, GPIO_PIN_9)

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

//OLED�����ú���

void oled_display_on(void);
void oled_display_off(void);	   							   		    
void oled_init(void);
void oled_clear(void);
void oled_user_clear(void);
void oled_draw_point(uint8_t x,uint8_t y,uint8_t t);
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void oled_show_int_number(uint8_t x,uint8_t y,int num,uint8_t Char_Size);
void oled_show_float_number(uint8_t x,uint8_t y,float num,uint8_t Char_Size);
void oled_show_string(uint8_t x,uint8_t y, uint8_t *p,uint8_t Char_Size);	 
void oled_set_pos(uint8_t x, uint8_t y);
void oled_show_chinese(uint8_t x,uint8_t y,uint8_t no);
void oled_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);

void fill_picture(uint8_t fill_Data);


void oled_temp_humi_display(float temp, float Humi);



#endif  
	 

