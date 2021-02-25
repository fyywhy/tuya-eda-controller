#include "gd32f10x.h"
#include "systick.h"
#include "led.h"
#include "serial.h"
#include "drv_ctrl.h"
#include "gd25qxx.h"
#include "i2c.h"
#include "wifi.h"
#include "oled.h"
//#include "drv_oled.h"
#include "sht30.h"
#include "mcu_info.h"

int main(void)
{
    int i = 0;
    float temp, humi;
	
	print_mcu_info();
	
    systick_config();
	led_init();
    oled_init();

    serial_init();   
    drv_ctrl_init();
    spi_flash_init(); 

    //i2c_config();
    wifi_protocol_init();
    //mcu_set_wifi_mode(SMART_CONFIG);

    sht30_init();
    oled_display_on();
    for (;;)
    {
        sht30_read_singleshot(&temp, &humi);		
		
        delay_1ms(1000);
        led_toggle();
        drv_ctrl_toggle();
        wifi_uart_service();

        mcu_dp_value_update(DPID_TEMP_CURRENT, temp * 10);
        mcu_dp_value_update(DPID_HUMIDITY_VALUE, humi);

        oled_user_clear();
        for (i = 0; i < 7; i++)
        {
            oled_show_chinese(i * 18, 0, i);
        }
        oled_temp_humi_display(temp, humi);
    }
}



