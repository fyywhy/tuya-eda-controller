#ifndef __SOFT_I2C_H_
#define __SOFT_I2C_H_

#include "gd32e23x.h"

/* 设置IIC_SDA的方向，输入还是输出 */
#define SDA_IN()  {gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);}//PB9输入模式
#define SDA_OUT() {gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);}//PB9输出模式

/* 设置SCL和SDA输出电平，以及读取SDA电平 */
#define IIC_SCL(n) (n?gpio_bit_set(GPIOA, GPIO_PIN_11):gpio_bit_reset(GPIOA, GPIO_PIN_11))
#define IIC_SDA(n) (n?gpio_bit_set(GPIOA, GPIO_PIN_12):gpio_bit_reset(GPIOA, GPIO_PIN_12))
#define READ_SDA	gpio_input_bit_get(GPIOA,GPIO_PIN_12)

void IIC_Init(void); 			/* 软件模拟IIC引脚初始化 */
void IIC_Start(void);			/* 启动I2C总线,即发送I2C起始条件 */
void IIC_Stop(void);			/* 结束I2C总线,即发送I2C结束条件 */
void IIC_ACK(void);				/* 发送应答 ACK */
void IIC_NACK(void);			/* 发送非应答 NACK */
uint8_t IIC_wait_ACK(void);		/* 等待ACK */
void IIC_SendByte(uint8_t byte);/* 一个字节数据发送函数 */ 
uint8_t IIC_RcvByte(void); 		/* 一个字节数据接收函数 */

#endif

