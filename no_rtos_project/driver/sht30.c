#include "sht30.h"
#include "systick.h"
#include <stdio.h>

/*****************************************************************
 *   GND   ��Դ��
 *   VCC   ��5V��3.3v��Դ
 *   SCL   ��PB10��SCL��
 *   SDA   ��PB11��SDA��            
 ******************************************************************/

#define write 0
#define read  1
uint8_t addr = 0x44;    //���ô�������ַ 

/* ����IIC_SDA�ķ������뻹����� */
#define SDA_IN()  {gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);}    //PB9����ģʽ
#define SDA_OUT() {gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);} //PB9���ģʽ

/* ����SCL��SDA�����ƽ���Լ���ȡSDA��ƽ */
#define IIC_SCL(n) (n?gpio_bit_set(GPIOB, GPIO_PIN_10):gpio_bit_reset(GPIOB, GPIO_PIN_10))
#define IIC_SDA(n) (n?gpio_bit_set(GPIOB, GPIO_PIN_11):gpio_bit_reset(GPIOB, GPIO_PIN_11))

#define READ_SDA    gpio_input_bit_get(GPIOB,GPIO_PIN_11)

/* ö��SHT3x�����б� */
typedef enum
{
    /* �����λ���� */
    SOFT_RESET_CMD = 0x30A2,
    /* ���β���ģʽ
     ������ʽ��Repeatability_CS_CMD
     CS�� Clock stretching */
    HIGH_ENABLED_CMD = 0x2C06, MEDIUM_ENABLED_CMD = 0x2C0D, LOW_ENABLED_CMD = 0x2C10,
    HIGH_DISABLED_CMD = 0x2400, MEDIUM_DISABLED_CMD = 0x240B, LOW_DISABLED_CMD = 0x2416,

    /* ���ڲ���ģʽ
     ������ʽ��Repeatability_MPS_CMD
     MPS��measurement per second */
    HIGH_0_5_CMD = 0x2032, MEDIUM_0_5_CMD = 0x2024, LOW_0_5_CMD = 0x202F,
    HIGH_1_CMD = 0x2130, MEDIUM_1_CMD = 0x2126, LOW_1_CMD = 0x212D,
    HIGH_2_CMD = 0x2236, MEDIUM_2_CMD = 0x2220, LOW_2_CMD = 0x222B,
    HIGH_4_CMD = 0x2334, MEDIUM_4_CMD = 0x2322, LOW_4_CMD = 0x2329,
    HIGH_10_CMD = 0x2737, MEDIUM_10_CMD = 0x2721, LOW_10_CMD = 0x272A,

    READOUT_FOR_PERIODIC_MODE = 0xE000, /* ���ڲ���ģʽ��ȡ�������� */
    READ_SERIAL_NUMBER = 0x3780, /* ��ȡ������������� */
} SHT30_CMD;

static void sht30_i2c_start(void); /* ����I2C����,������I2C��ʼ���� */
static void sht30_i2c_stop(void); /* ����I2C����,������I2C�������� */
static void sht30_i2c_ack(void); /* ����Ӧ�� ACK */
static void sht30_i2c_nack(void); /* ���ͷ�Ӧ�� NACK */
static uint8_t i2c_wait_ack(void); /* �ȴ�ACK */
static void sht30_i2c_send_byte(uint8_t byte);/* һ���ֽ����ݷ��ͺ��� */
static uint8_t sht30_i2c_recv_byte(void); /* һ���ֽ����ݽ��պ��� */

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

/* ��������ȡ���������
 * �������洢������ݵ�ָ��
 * ����ֵ��0-��ȡ�ɹ���1-��ȡʧ�� */
uint8_t sht30_read_serial_number(uint32_t* sn)
{
    uint8_t ret = 0;
    uint8_t sn_buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

    sht30_i2c_start();
    sht30_send_cmd(READ_SERIAL_NUMBER);
    sht30_i2c_stop();
    delay_1ms(10); /* ������ʱ��Ҫ�ʵ��ӳ�������������*/
    sht30_i2c_start();
    ret = sht30_recv_data(4, sn_buffer);
    sht30_i2c_stop();

    *sn = ((sn_buffer[0] << 24) | (sn_buffer[1] << 16) | (sn_buffer[2] << 8) | (sn_buffer[3]));
    if (0xFF == *sn)
        return 1;
    return ret;
}

/* ����������CRCУ��
 * ����message����ҪУ�������
 * ����initial_value��crc��ʼֵ
 * ����ֵ������õ���CRC�� */
#define CRC8_POLYNOMIAL 0x131
uint8_t check_crc_8bit(uint8_t* const message, uint8_t initial_value)
{
    uint8_t remainder;     //����
    uint8_t i = 0, j = 0;  //ѭ������

    /* ��ʼ�� */
    remainder = initial_value;
    for (j = 0; j < 2; j++)
    {
        remainder ^= message[j];
        /* �����λ��ʼ���μ���  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
                remainder = (remainder << 1) ^ CRC8_POLYNOMIAL;
            else
                remainder = (remainder << 1);
        }
    }
    /* ���ؼ����CRC�� */
    return remainder;
}

/* ��������ʪ�����ݻ�ȡ����,���ڶ�ȡ��ע�⣬��Ҫ��ǰ��������ģʽ   
 * ����temp_val���洢�¶����ݵ�ָ��, �¶ȵ�λΪ��C
 * ����humi_val���洢ʪ�����ݵ�ָ��, �¶ȵ�λΪ%
 * ����ֵ��0-��ȡ�ɹ���1-��ȡʧ��
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

    /* У���¶����ݺ�ʪ�������Ƿ������ȷ */
    if (check_crc_8bit(buff, 0xFF) != buff[2] || check_crc_8bit(&buff[3], 0xFF) != buff[5])
    {
        printf("CRC_ERROR,ret = 0x%x\r\n", ret);
        return 1;
    }

    /* ת���¶����� */
    tem = (((uint16_t) buff[0] << 8) | buff[1]);  //�¶�����ƴ��
    Temperature = (175.0 * (float) tem / 65535.0 - 45.0);  // T = -45 + 175 * tem / (2^16-1)

    /* ת��ʪ������ */
    hum = (((uint16_t) buff[3] << 8) | buff[4]);  //ʪ������ƴ��
    Humidity = (100.0 * (float) hum / 65535.0);           // RH = hum*100 / (2^16-1)

    /* ���˴������� */
    if ((Temperature >= -20) && (Temperature <= 125) && (Humidity >= 0) && (Humidity <= 100))
    {
        *temp_val = Temperature;
        *humi_val = Humidity;
        return 0;
    }
    else
        return 1;
}
/* ��������ʪ�����ݻ�ȡ���������λ�ȡ
 * ����temp_val���洢�¶����ݵ�ָ��, �¶ȵ�λΪ��C
 * ����humi_val���洢ʪ�����ݵ�ָ��, �¶ȵ�λΪ%
 * ����ֵ��0-��ȡ�ɹ���1-��ȡʧ��
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

    /* У���¶����ݺ�ʪ�������Ƿ������ȷ */
    if (check_crc_8bit(buff, 0xFF) != buff[2] || check_crc_8bit(&buff[3], 0xFF) != buff[5])
    {
        printf("CRC_ERROR,ret = 0x%x\r\n", ret);
        return 1;
    }

    /* ת���¶����� */
    tem = (((uint16_t) buff[0] << 8) | buff[1]);           //�¶�����ƴ��
    Temperature = (175.0 * (float) tem / 65535.0 - 45.0);  // T = -45 + 175 * tem / (2^16-1)

    /* ת��ʪ������ */
    hum = (((uint16_t) buff[3] << 8) | buff[4]);  //ʪ������ƴ��
    Humidity = (100.0 * (float) hum / 65535.0);   // RH = hum*100 / (2^16-1)

    /* ���˴������� */
    if ((Temperature >= -20) && (Temperature <= 125) && (Humidity >= 0) && (Humidity <= 100))
    {
        *temp_val = Temperature;
        *humi_val = Humidity;
        return 0;
    }
    else
        return 1;
}

/* ����������I2C����,������I2C��ʼ����. 
 * ������  ��
 * ����ֵ����                        */
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

/* ����������I2C����,������I2C��������.  
 * ������  ��
 * ����ֵ����                        */
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

/* ����������Ӧ�� ACK 
 * ������  ��
 * ����ֵ����        */
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

/* ���������ͷ�Ӧ�� NACK 
 * ������  ��
 * ����ֵ����        */
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

/* �������ȴ�ACK 
 * ������  ��
 * ����ֵ���ȴ�Ӧ�𷵻�0��û�еȴ���Ӧ�𷵻�1   */
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
    ; /* ���ݷ�������ͷ������ߣ�׼������Ӧ��λ */
    delay_1ms(1);
    while (READ_SDA) /* �ȴ�IICӦ��*/
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

/* ������һ���ֽ����ݷ��ͺ���               
 * ������  ��
 * ����ֵ����        */
static void sht30_i2c_send_byte(uint8_t byte)
{
    uint8_t BitCnt;
    SDA_OUT()
    ;
    IIC_SCL(0);
    for (BitCnt = 0; BitCnt < 8; BitCnt++) /* Ҫ���͵����ݳ���Ϊ8λ */
    {
        if (byte & 0x80)
            IIC_SDA(1); /* �жϷ���λ */
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

/* ������һ���ֽ����ݽ��պ���               
 * ������  ��
 * ����ֵ�����յ����ֽ�����     */
static uint8_t sht30_i2c_recv_byte(void)
{
    uint8_t retc;
    uint8_t BitCnt;
    retc = 0;
    SDA_IN()
    ; /* ����������Ϊ���뷽ʽ */
    delay_1ms(1);
    for (BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        IIC_SCL(0); /* ����ʱ����Ϊ�ͣ�׼����������λ  */
        delay_1ms(2);
        IIC_SCL(1); /* ����ʱ����Ϊ��ʹ��������������Ч  */
        retc = retc << 1;
        if (READ_SDA)
            retc |= 1; /* ������λ,���յ�����λ����retc�� */
        delay_1ms(1);
    }
    IIC_SCL(0);
    return (retc);
}

/* ��������SHT30����һ��16bitָ�� 
 * ����cmd��SHT30ָ���SHT30_MODE��ö�ٶ��壩
 * ����ֵ�����ͳɹ�����0������ʧ�ܷ���1      */
static uint8_t sht30_send_cmd(SHT30_CMD cmd)
{
    uint8_t cmd_buffer[2];
    uint8_t ret;
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;

    sht30_i2c_send_byte(addr << 1 | write); /* д7λI2C�豸��ַ��0��Ϊдȡλ */
    ret = i2c_wait_ack();
    sht30_i2c_send_byte(cmd_buffer[0]);
    ret |= i2c_wait_ack();
    sht30_i2c_send_byte(cmd_buffer[1]);
    ret |= i2c_wait_ack();

    return ret;
}

/* ��������SHT3x��ȡ���� 
 * ����data_len����ȡ���ٸ��ֽ�����
 * ����data_arr����ȡ�����ݴ����һ��������
 * ����ֵ����ȡ�ɹ�����0����ȡʧ�ܷ���1 
 */
static uint8_t sht30_recv_data(uint8_t data_len, uint8_t* data_arr)
{
    uint8_t ret, i;
    sht30_i2c_send_byte(addr << 1 | read); /* д7λI2C�豸��ַ��1Ϊ��ȡλ */
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

