
#include "i2c.h"
#include <stdio.h>

//for oled
static void i2c0_config(void)
{
	/* enable AF clock */
	rcu_periph_clock_enable(RCU_AF);
	
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
	
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);

    /* connect PB8 to I2C0_SCL */
    /* connect PB9 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9);
	
	gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
	
    /* configure I2C clock */
    i2c_clock_config(I2C0,I2C0_SPEED,I2C_DTCY_2);
	
    /* configure I2C address */
    i2c_mode_addr_config(I2C0,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,I2C0_SLAVE_ADDRESS7);
   
   /* enable I2C0 */
    i2c_enable(I2C0);
	
    /* enable acknowledge */
    i2c_ack_config(I2C0,I2C_ACK_ENABLE);
}

//for sht30
static void i2c1_config(void)
{
	/* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C1 clock */
    rcu_periph_clock_enable(RCU_I2C1);

    /* connect PB10 to I2C1_SCL */
    /* connect PB11 to I2C1_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);
	
    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C1);
    /* configure I2C clock */
    i2c_clock_config(I2C1,I2C1_SPEED,I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C1,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,I2C1_SLAVE_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C1);
    /* enable acknowledge */
    i2c_ack_config(I2C1,I2C_ACK_ENABLE);
}

void i2c_config(void)
{
	i2c0_config();
	i2c1_config();
}
