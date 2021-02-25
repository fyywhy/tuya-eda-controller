#include "drv_spi.h"
#include "gd32f10x.h"

#define  ARRAYSIZE         10

uint8_t spi0_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
uint8_t spi0_receive_array[ARRAYSIZE]; 


static void rcu_config(void);
static void gpio_config(void);
static void dma_config(void);
static void spi_config(void);

void drv_spi_init(void)
{
    /* peripheral clock enable */
    rcu_config();
    /* GPIO configure */
    gpio_config();
    /* DMA configure */
    dma_config();
    /* SPI configure */
    spi_config();

    /* SPI enable */
    spi_enable(SPI0);

    /* DMA channel enable */
    dma_channel_enable(DMA0, DMA_CH1);
    dma_channel_enable(DMA0, DMA_CH2);

    /* SPI DMA enable */
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
    spi_dma_enable(SPI0, SPI_DMA_RECEIVE);

    /* wait dma transmit complete */
    while(!dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF));
    while(!dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF));
}

static void rcu_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI0);
}

static void gpio_config(void)
{
	/* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
}

static void dma_config(void)
{
	dma_parameter_struct dma_init_struct;
    
    /* SPI0 transmit dma config:DMA0-DMA_CH2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_struct_para_init(&dma_init_struct);
    
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)spi0_send_array;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = ARRAYSIZE;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);

    /* SPI0 receive dma config:DMA0-DMA_CH1 */
    dma_deinit(DMA0, DMA_CH1);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)spi0_receive_array;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);
}

static void spi_config(void)
{
	spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
    
    spi_nss_internal_high(SPI0);
}

