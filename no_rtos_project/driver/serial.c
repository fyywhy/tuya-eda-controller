#include "serial.h"

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}


static void serial0_init(void)
{
	/* enable AF clock */
	rcu_periph_clock_enable(RCU_AF);
	
	/* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
	
    /* connect port to USARTx_Tx */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	
	gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
	
    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
	
	printf("\r\n usart0 init done.\r\n");
}

static void serial1_init(void)
{
	/* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* USART configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 9600U);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_enable(USART1);
	
	printf(" usart1 init done.\r\n");
}

void serial_init(void)
{
	serial0_init();
	
	/* configure COM1 */
    serial1_init();	
		
	/* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0, 0);
	nvic_irq_enable(USART1_IRQn, 0, 1);
	
	/* enable USART0 receive interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    
	/* enable USART1 receive interrupt */
    usart_interrupt_enable(USART1, USART_INT_RBNE);
	
//	
//	/* enable USART0 transmit interrupt */
//    usart_interrupt_enable(USART1, USART_INT_TBE);
}


/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart1_send_data(unsigned char value)
{
	while (RESET == usart_flag_get(USART1, USART_FLAG_TBE));
	usart_data_transmit(USART1, (uint8_t)value);
	while (RESET == usart_flag_get(USART1, USART_FLAG_TC));
}
