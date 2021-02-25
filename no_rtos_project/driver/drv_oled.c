//  文 件 名   : oled.c
//  版 本 号   : v1.0
//  作    者   : fyywhy
//  编写日期   : 2020-02-22
//  功能描述   : 0.96寸OLED驱动程序

/*
*----------------------------------------------------------------
*	GND   电源地
*	VCC   接5V或3.3v电源
*	SCL   接PB8（SCL）
*	SDA   接PB9（SDA）            
* ----------------------------------------------------------------
*/

#include "drv_oled.h"
#include "oledfont.h"  
#include "systick.h"

uint8_t oled_init_cmd[]={
0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,					
0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,					
0xD8, 0x30, 0x8D, 0x14, 0xAF};

uint8_t *p_buffer = NULL;

static void write_i2c_cmd(uint8_t cmd);
static void write_i2c_data(uint8_t data);
static void oled_write_byte(unsigned dat,unsigned cmd);
static void oled_set_pos(uint8_t x, uint8_t y);

void oled_init(void)
{
	uint8_t size = 27;
	p_buffer = oled_init_cmd;
	
	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    /* send the internal address to write to : only one byte address */
    i2c_data_transmit(I2C0, 0x00);
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    /* while there is data to be written */
    while(size--){  
        i2c_data_transmit(I2C0, *p_buffer);
        
        /* point to the next byte to be written */
        p_buffer++; 
        
        /* wait until BTC bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    
    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C0)&0x0200);
}


/**********************************************
// IIC Write Command
**********************************************/
static void write_i2c_cmd(uint8_t cmd)
{
	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    /* send the internal address to write to : only one byte address */
    i2c_data_transmit(I2C0, 0x00);
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
	
	i2c_data_transmit(I2C0, cmd);
	
	/* wait until BTC bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)){}
    
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    
    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C0)&0x0200);
}
/**********************************************
// IIC Write Data
**********************************************/
static void write_i2c_data(uint8_t data)
{		
	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    /* send the internal address to write to : only one byte address */
    i2c_data_transmit(I2C0, 0x00);
    
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
	
	i2c_data_transmit(I2C0, data);
	
	/* wait until BTC bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)){}
    
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    
    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C0)&0x0200);
}

static void oled_write_byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		write_i2c_data(dat);
	}
	else 
	{
		write_i2c_cmd(dat);
	}
}

//坐标设置
static void oled_set_pos(uint8_t x, uint8_t y) 
{
 	oled_write_byte(0xb0+y,OLED_CMD);
	oled_write_byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	oled_write_byte((x&0x0f),OLED_CMD); 
}   	  
//开启OLED显示    
void oled_display_on(void)
{
	oled_write_byte(0X8D,OLED_CMD);  //SET DCDC命令
	oled_write_byte(0X14,OLED_CMD);  //DCDC ON
	oled_write_byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void oled_display_off(void)
{
	oled_write_byte(0X8D,OLED_CMD);  //SET DCDC命令
	oled_write_byte(0X10,OLED_CMD);  //DCDC OFF
	oled_write_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void oled_clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		oled_write_byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		oled_write_byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		oled_write_byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)oled_write_byte(0,OLED_DATA); 
	} //更新显示
}
void oled_on(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		oled_write_byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		oled_write_byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		oled_write_byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)oled_write_byte(1,OLED_DATA); 
	} //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t char_size)
{      	
	  unsigned char c=0,i=0;	
		c = chr - ' ' ;  //得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(char_size ==16)
			{
				oled_set_pos(x,y);	
				for(i=0;i<8;i++)
				oled_write_byte(F8X16[c*16+i],OLED_DATA);
				oled_set_pos(x,y+1);
				for(i=0;i<8;i++)
				oled_write_byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				oled_set_pos(x,y);
				for(i=0;i<6;i++)
				oled_write_byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				oled_show_char(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	oled_show_char(x+(size2/2)*t,y,temp+'0',size2); 
	}
}


void oled_show_int_number(uint8_t x,uint8_t y,int num,uint8_t char_size)  //显示整数，包含正负
{
	unsigned char len=0;
	int buf=num;
	while(buf)
	{
		len++;
		buf/=10;
	}
	if(num>0)
		oled_show_number(x,y,num,len,char_size);
	else
	{
		oled_show_char(x,y,'-',char_size);
		num=abs(num);
		if(char_size==16)
			oled_show_number(x+8,y,num,len,char_size);
		else
			oled_show_number(x+6,y,num,len,char_size);
	}
}

/*
描述：显示整数(可显示正负数)（-65535~65535）
输入：
	x:显示的x起始位置
	y:显示的y起始位置
	num:要显示的数字
	char_size:要显示数字的大小
局限：目前只能显示确定范围的小数，且小数部分显示不准确
*/
void oled_show_float_number(uint8_t x,uint8_t y,float num,uint8_t char_size)
{
	unsigned char zheng_len=0,xiao_len=0,polarity_flag;
	int zheng=0,xiao=0;
	if(num>0)
		polarity_flag=1;
	else
	{
		polarity_flag=0;
		num=-num;
	}
	zheng=(int)num;
	xiao=((num-zheng)*100)/1;	//显示小数点后两位
	while(zheng)	//计算整数部分位数
	{
		zheng_len++;
		zheng/=10;
	}
	xiao_len=2;
	zheng=(int)num;	//在计算整数部分数值时，原赋值被篡改，这里重新赋值
	if(polarity_flag)
	{	
		if(char_size==16)
		{
			oled_show_number(x,y,zheng,zheng_len,char_size);
			oled_show_char(x+zheng_len*8,y,'.',char_size);
			oled_show_number(x+(zheng_len+1)*8,y,xiao,xiao_len,char_size);
		}
		else
		{
			oled_show_number(x,y,zheng,zheng_len,char_size);
			oled_show_char(x+zheng_len*6,y,'.',char_size);
			oled_show_number(x+(zheng_len+1)*6,y,xiao,xiao_len,char_size);
		}
	}
	else
	{
		oled_show_char(x,y,'-',char_size);
		if(char_size==16)
		{
			oled_show_number(x+8,y,zheng,zheng_len,char_size);
			oled_show_char(x+(zheng_len+1)*8,y,'.',char_size);
			oled_show_number(x+(zheng_len+2)*8,y,xiao,xiao_len,char_size);
		}
		else
		{
			oled_show_number(x+6,y,zheng,zheng_len,char_size);
			oled_show_char(x+(zheng_len+1)*6,y,'.',char_size);
			oled_show_number(x+(zheng_len+2)*6,y,xiao,xiao_len,char_size);
		}
	}
}

//显示一个字符号串
void oled_show_string(uint8_t x,uint8_t y,uint8_t *chr,uint8_t char_size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		oled_show_char(x,y,chr[j],char_size);
		x+=8;
		if(x>120)
		{
			x=0;
			y+=2;
		}
		j++;
	}
}

//显示汉字
void oled_show_chinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	oled_set_pos(x,y);	
	for(t=0;t<16;t++)
	{
		oled_write_byte(Hzk[2*no][t],OLED_DATA);
		adder+=1;
	}	
	oled_set_pos(x,y+1);	
	for(t=0;t<16;t++)
	{	
		oled_write_byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
	}					
}


