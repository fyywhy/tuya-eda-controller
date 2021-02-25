#include "sht30.h"
#include "systick.h"
#include <stdio.h>

/*****************************************************************
 *   GND   电源地
 *   VCC   接5V或3.3v电源
 *   SCL   接PB10（SCL）
 *   SDA   接PB11（SDA）            
 ******************************************************************/

#define write 0
#define read  1
uint8_t addr = 0x44;    //设置传感器地址 

/* 设置IIC_SDA的方向，输入还是输出 */
#define SDA_IN()  {gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);}    //PB9输入模式
#define SDA_OUT() {gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);} //PB9输出模式

/* 设置SCL和SDA输出电平，以及读取SDA电平 */
#define IIC_SCL(n) (n?gpio_bit_set(GPIOB, GPIO_PIN_10):gpio_bit_reset(GPIOB, GPIO_PIN_10))
#define IIC_SDA(n) (n?gpio_bit_set(GPIOB, GPIO_PIN_11):gpio_bit_reset(GPIOB, GPIO_PIN_11))

#define READ_SDA    gpio_input_bit_get(GPIOB,GPIO_PIN_11)

/* 枚举SHT3x命令列表 */
typedef enum
{
    /* 软件复位命令 */
    SOFT_RESET_CMD = 0x30A2,
    /* 单次测量模式
     命名格式：Repeatability_CS_CMD
     CS： Clock stretching */
    HIGH_ENABLED_CMD = 0x2C06, MEDIUM_ENABLED_CMD = 0x2C0D, LOW_ENABLED_CMD = 0x2C10,
    HIGH_DISABLED_CMD = 0x2400, MEDIUM_DISABLED_CMD = 0x240B, LOW_DISABLED_CMD = 0x2416,

    /* 周期测量模式
     命名格式：Repeatability_MPS_CMD
     MPS：measurement per second */
    HIGH_0_5_CMD = 0x2032, MEDIUM_0_5_CMD = 0x2024, LOW_0_5_CMD = 0x202F,
    HIGH_1_CMD = 0x2130, MEDIUM_1_CMD = 0x2126, LOW_1_CMD = 0x212D,
    HIGH_2_CMD = 0x2236, MEDIUM_2_CMD = 0x2220, LOW_2_CMD = 0x222B,
    HIGH_4_CMD = 0x2334, MEDIUM_4_CMD = 0x2322, LOW_4_CMD = 0x2329,
    HIGH_10_CMD = 0x2737, MEDIUM_10_CMD = 0x2721, LOW_10_CMD = 0x272A,

    READOUT_FOR_PERIODIC_MODE = 0xE000, /* 周期测量模式读取数据命令 */
    READ_SERIAL_NUMBER = 0x3780, /* 读取传感器编号命令 */
} SHT30_CMD;

static void sht30_i2c_start(void); /* 启动I2C总线,即发送I2C起始条件 */
static void sht30_i2c_stop(void); /* 结束I2C总线,即发送I2C结束条件 */
static void sht30_i2c_ack(void); /* 发送应答 ACK */
static void sht30_i2c_nack(void); /* 发送非应答 NACK */
static uint8_t i2c_wait_ack(void); /* 等待ACK */
static void sht30_i2c_send_byte(uint8_t byte);/* 一个字节数据发送函数 */
static uint8_t sht30_i2c_recv_byte(void); /* 一个字节数据接收函数 */

static uint8_t sht30_send_cmd(SHT30_CMD cmd);
static uint8_t sht30_recv_data(uint8_t data_len, uint8_t* data_arr);


void sht30_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    GPIO_BOP (GPIOB) = GPIO_PIN_10;
    GPIO_BOP (GPIOB) = GPIO_PIN_11;
}

/* 描述：读取传感器编号
 * 参数：存储编号数据的指针
 * 返回值：0-读取成功，1-读取失败 */
uint8_t sht30_read_serial_number(uint32_t* sn)
{
    uint8_t ret = 0;
    uint8_t sn_buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

    sht30_i2c_start();
    sht30_send_cmd(READ_SERIAL_NUMBER);
    sht30_i2c_stop();
    delay_1ms(10); /* 有问题时需要适当延长！！！！！！*/
    sht30_i2c_start();
    ret = sht30_recv_data(4, sn_buffer);
    sht30_i2c_stop();

    *sn = ((sn_buffer[0] << 24) | (sn_buffer[1] << 16) | (sn_buffer[2] << 8) | (sn_buffer[3]));
    if (0xFF == *sn)
        return 1;
    return ret;
}

/* 描述：数据CRC校验
 * 参数message：需要校验的数据
 * 参数initial_value：crc初始值
 * 返回值：计算得到的CRC码 */
#define CRC8_POLYNOMIAL 0x131
uint8_t check_crc_8bit(uint8_t* const message, uint8_t initial_value)
{
    uint8_t remainder;     //余数
    uint8_t i = 0, j = 0;  //循环变量

    /* 初始化 */
    remainder = initial_value;
    for (j = 0; j < 2; j++)
    {
        remainder ^= message[j];
        /* 从最高位开始依次计算  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
                remainder = (remainder << 1) ^ CRC8_POLYNOMIAL;
            else
                remainder = (remainder << 1);
        }
    }
    /* 返回计算的CRC码 */
    return remainder;
}

/* 描述：温湿度数据获取函数,周期读取，注意，需要提前设置周期模式   
 * 参数temp_val：存储温度数据的指针, 温度单位为°C
 * 参数humi_val：存储湿度数据的指针, 温度单位为%
 * 返回值：0-读取成功，1-读取失败
 ********************************************************************/
uint8_t sht30_read_periodic(float *temp_val, float *humi_val)
{
    uint8_t ret = 0;
    uint8_t buff[6];
    uint16_t tem, hum;
    float Temperature = 0;
    float Humidity = 0;

    sht30_i2c_start();
    ret = sht30_send_cmd(READOUT_FOR_PERIODIC_MODE);
    sht30_i2c_start();
    ret = sht30_recv_data(6, buff);
    sht30_i2c_stop();

    /* 校验温度数据和湿度数据是否接收正确 */
    if (check_crc_8bit(buff, 0xFF) != buff[2] || check_crc_8bit(&buff[3], 0xFF) != buff[5])
    {
        printf("CRC_ERROR,ret = 0x%x\r\n", ret);
        return 1;
    }

    /* 转换温度数据 */
    tem = (((uint16_t) buff[0] << 8) | buff[1]);  //温度数据拼接
    Temperature = (175.0 * (float) tem / 65535.0 - 45.0);  // T = -45 + 175 * tem / (2^16-1)

    /* 转换湿度数据 */
    hum = (((uint16_t) buff[3] << 8) | buff[4]);  //湿度数据拼接
    Humidity = (100.0 * (float) hum / 65535.0);           // RH = hum*100 / (2^16-1)

    /* 过滤错误数据 */
    if ((Temperature >= -20) && (Temperature <= 125) && (Humidity >= 0) && (Humidity <= 100))
    {
        *temp_val = Temperature;
        *humi_val = Humidity;
        return 0;
    }
    else
        return 1;
}
/* 描述：温湿度数据获取函数，单次获取
 * 参数temp_val：存储温度数据的指针, 温度单位为°C
 * 参数humi_val：存储湿度数据的指针, 温度单位为%
 * 返回值：0-读取成功，1-读取失败
 ********************************************************************/
uint8_t sht30_read_singleshot(float *temp_val, float *humi_val)
{
    uint8_t ret = 0;
    uint8_t buff[6];
    uint16_t tem, hum;
    float Temperature = 0;
    float Humidity = 0;

    sht30_i2c_start();
    sht30_send_cmd(HIGH_ENABLED_CMD);
    sht30_i2c_stop();

    delay_1ms(50);

    sht30_i2c_start();
    ret = sht30_recv_data(6, buff);
    sht30_i2c_stop();

    /* 校验温度数据和湿度数据是否接收正确 */
    if (check_crc_8bit(buff, 0xFF) != buff[2] || check_crc_8bit(&buff[3], 0xFF) != buff[5])
    {
        printf("CRC_ERROR,ret = 0x%x\r\n", ret);
        return 1;
    }

    /* 转换温度数据 */
    tem = (((uint16_t) buff[0] << 8) | buff[1]);           //温度数据拼接
    Temperature = (175.0 * (float) tem / 65535.0 - 45.0);  // T = -45 + 175 * tem / (2^16-1)

    /* 转换湿度数据 */
    hum = (((uint16_t) buff[3] << 8) | buff[4]);  //湿度数据拼接
    Humidity = (100.0 * (float) hum / 65535.0);   // RH = hum*100 / (2^16-1)

    /* 过滤错误数据 */
    if ((Temperature >= -20) && (Temperature <= 125) && (Humidity >= 0) && (Humidity <= 100))
    {
        *temp_val = Temperature;
        *humi_val = Humidity;
        return 0;
    }
    else
        return 1;
}

/* 描述：启动I2C总线,即发送I2C起始条件. 
 * 参数：  无
 * 返回值：无                        */
static void sht30_i2c_start(void)
{
    SDA_OUT();
    IIC_SDA(1);
    IIC_SCL(1);
    delay_1ms(4);
    IIC_SDA(0);
    delay_1ms(4);
    IIC_SCL(0);
}

/* 描述：结束I2C总线,即发送I2C结束条件.  
 * 参数：  无
 * 返回值：无                        */
static void sht30_i2c_stop(void)
{
    SDA_OUT();
    IIC_SCL(0);
    IIC_SDA(0);
    delay_1ms(4);
    IIC_SCL(1);
    delay_1ms(4);
    IIC_SDA(1);
    delay_1ms(4);
}

/* 描述：发送应答 ACK 
 * 参数：  无
 * 返回值：无        */
static void sht30_i2c_ack(void)
{
    SDA_OUT()
    ;
    IIC_SCL(0);
    delay_1ms(2);
    IIC_SDA(0);
    delay_1ms(2);
    IIC_SCL(1);
    delay_1ms(2);
    IIC_SCL(0);
    delay_1ms(1);
}

/* 描述：发送非应答 NACK 
 * 参数：  无
 * 返回值：无        */
static void sht30_i2c_nack(void)
{
    SDA_OUT()
    ;
    IIC_SCL(0);
    delay_1ms(2);
    IIC_SDA(1);
    delay_1ms(2);
    IIC_SCL(1);
    delay_1ms(2);
    IIC_SCL(0);
    delay_1ms(1);
}

/* 描述：等待ACK 
 * 参数：  无
 * 返回值：等待应答返回0，没有等待到应答返回1   */
static uint8_t i2c_wait_ack(void)
{
    uint8_t t = 200;
    SDA_OUT()
    ;
    IIC_SDA(1);
    delay_1ms(1);
    IIC_SCL(0);
    delay_1ms(1);
    SDA_IN()
    ; /* 数据发送完后释放数据线，准备接收应答位 */
    delay_1ms(1);
    while (READ_SDA) /* 等待IIC应答*/
    {
        t--;
        delay_1ms(1);
        if (t == 0)
        {
            IIC_SCL(0);
            return 1;
        }
        delay_1ms(1);
    }
    delay_1ms(1);
    IIC_SCL(1);
    delay_1ms(1);
    IIC_SCL(0);
    delay_1ms(1);
    return 0;
}

/* 描述：一个字节数据发送函数               
 * 参数：  无
 * 返回值：无        */
static void sht30_i2c_send_byte(uint8_t byte)
{
    uint8_t BitCnt;
    SDA_OUT()
    ;
    IIC_SCL(0);
    for (BitCnt = 0; BitCnt < 8; BitCnt++) /* 要传送的数据长度为8位 */
    {
        if (byte & 0x80)
            IIC_SDA(1); /* 判断发送位 */
        else
            IIC_SDA(0);
        byte <<= 1;
        delay_1ms(2);
        IIC_SCL(1);
        delay_1ms(2);
        IIC_SCL(0);
        delay_1ms(2);
    }
}

/* 描述：一个字节数据接收函数               
 * 参数：  无
 * 返回值：接收到的字节数据     */
static uint8_t sht30_i2c_recv_byte(void)
{
    uint8_t retc;
    uint8_t BitCnt;
    retc = 0;
    SDA_IN()
    ; /* 设置数据线为输入方式 */
    delay_1ms(1);
    for (BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        IIC_SCL(0); /* 设置时钟线为低，准备接收数据位  */
        delay_1ms(2);
        IIC_SCL(1); /* 设置时钟线为高使数据线上数据有效  */
        retc = retc << 1;
        if (READ_SDA)
            retc |= 1; /* 读数据位,接收的数据位放入retc中 */
        delay_1ms(1);
    }
    IIC_SCL(0);
    return (retc);
}

/* 描述：向SHT30发送一条16bit指令 
 * 参数cmd：SHT30指令（在SHT30_MODE中枚举定义）
 * 返回值：发送成功返回0，发送失败返回1      */
static uint8_t sht30_send_cmd(SHT30_CMD cmd)
{
    uint8_t cmd_buffer[2];
    uint8_t ret;
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;

    sht30_i2c_send_byte(addr << 1 | write); /* 写7位I2C设备地址加0作为写取位 */
    ret = i2c_wait_ack();
    sht30_i2c_send_byte(cmd_buffer[0]);
    ret |= i2c_wait_ack();
    sht30_i2c_send_byte(cmd_buffer[1]);
    ret |= i2c_wait_ack();

    return ret;
}

/* 描述：从SHT3x读取数据 
 * 参数data_len：读取多少个字节数据
 * 参数data_arr：读取的数据存放在一个数组里
 * 返回值：读取成功返回0，读取失败返回1 
 */
static uint8_t sht30_recv_data(uint8_t data_len, uint8_t* data_arr)
{
    uint8_t ret, i;
    sht30_i2c_send_byte(addr << 1 | read); /* 写7位I2C设备地址加1为读取位 */
    ret = i2c_wait_ack();
    if (ret != 0)
        return 1;
    for (i = 0; i < (data_len - 1); i++)
    {
        data_arr[i] = sht30_i2c_recv_byte();
        sht30_i2c_ack();
    }
    data_arr[i] = sht30_i2c_recv_byte();
    sht30_i2c_nack();

    return 0;
}

