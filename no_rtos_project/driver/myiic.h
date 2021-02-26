#ifndef __SOFT_I2C_H_
#define __SOFT_I2C_H_

#include "gd32e23x.h"

/* ����IIC_SDA�ķ������뻹����� */
#define SDA_IN()  {gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);}//PB9����ģʽ
#define SDA_OUT() {gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);}//PB9���ģʽ

/* ����SCL��SDA�����ƽ���Լ���ȡSDA��ƽ */
#define IIC_SCL(n) (n?gpio_bit_set(GPIOA, GPIO_PIN_11):gpio_bit_reset(GPIOA, GPIO_PIN_11))
#define IIC_SDA(n) (n?gpio_bit_set(GPIOA, GPIO_PIN_12):gpio_bit_reset(GPIOA, GPIO_PIN_12))
#define READ_SDA	gpio_input_bit_get(GPIOA,GPIO_PIN_12)

void IIC_Init(void); 			/* ���ģ��IIC���ų�ʼ�� */
void IIC_Start(void);			/* ����I2C����,������I2C��ʼ���� */
void IIC_Stop(void);			/* ����I2C����,������I2C�������� */
void IIC_ACK(void);				/* ����Ӧ�� ACK */
void IIC_NACK(void);			/* ���ͷ�Ӧ�� NACK */
uint8_t IIC_wait_ACK(void);		/* �ȴ�ACK */
void IIC_SendByte(uint8_t byte);/* һ���ֽ����ݷ��ͺ��� */ 
uint8_t IIC_RcvByte(void); 		/* һ���ֽ����ݽ��պ��� */

#endif

