
#include "gd32f10x.h"
#include "led.h"

void led_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

	GPIO_BC(GPIOB) = GPIO_PIN_15;
	GPIO_BC(GPIOB) = GPIO_PIN_14;
	GPIO_BC(GPIOB) = GPIO_PIN_13;
}

void led_toggle(void)
{
	static int is_on = 0;
	is_on = !is_on;
	led_switch(is_on);
}

void led_switch(int is_on)
{
	if(is_on)
	{
		GPIO_BOP(GPIOB) = GPIO_PIN_15;
		GPIO_BOP(GPIOB) = GPIO_PIN_14;
		GPIO_BOP(GPIOB) = GPIO_PIN_13;
		return;
	}
	
	GPIO_BC(GPIOB) = GPIO_PIN_15;
	GPIO_BC(GPIOB) = GPIO_PIN_14;
	GPIO_BC(GPIOB) = GPIO_PIN_13;
}



