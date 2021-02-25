#include "drv_ctrl.h"
#include "gd32f10x.h"

void drv_ctrl_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	
    GPIO_BC(GPIOA) = GPIO_PIN_9;
	GPIO_BC(GPIOA) = GPIO_PIN_10;
	GPIO_BC(GPIOA) = GPIO_PIN_11;
	GPIO_BC(GPIOA) = GPIO_PIN_12;
}

void drv_ctrl_toggle(void)
{
	static int is_on = 0;
	is_on = !is_on;
	drv_ctrl_switch(is_on);
}

void drv_ctrl_switch(int is_on)
{
	if(is_on)
	{
		GPIO_BOP(GPIOA) = GPIO_PIN_9;
		GPIO_BOP(GPIOA) = GPIO_PIN_10;
		GPIO_BOP(GPIOA) = GPIO_PIN_11;
		GPIO_BOP(GPIOA) = GPIO_PIN_12;
		return;
	}
	GPIO_BC(GPIOA) = GPIO_PIN_9;
	GPIO_BC(GPIOA) = GPIO_PIN_10;
	GPIO_BC(GPIOA) = GPIO_PIN_11;
	GPIO_BC(GPIOA) = GPIO_PIN_12;
}



