# 立创&涂鸦训练营第二期--远程温湿度感应控制器

### 基于温湿度传感器，自动采集当前环境的温湿度，并通过GD32 & wifi模块上传至云端，送至APP端显示.

- This project is developed using Tuya SDK, which enables you to quickly develop branded apps connecting and controlling smart scenarios of many devices. For more information, please check Tuya Developer Website.

------

## 关键词

GD32 WIFI 物联网 

------

### 整体介绍

- 电路原理图、PCB板设计，请移步[立创开源平台工程](https://oshwhub.com/fyywhy/wu-lian-wang-wen-shi-du-ji)

------

### 硬件电路介绍

- 供电电路：
  1. 可通过3.7V锂电池或micro  usb供电，在断开micro usb电源后自动接通锂电池供电。
  2. 通过右下方的轻触按键实现一键开关机，电源流入后，经过LDO电路降压到3.3V后供给整个系统使用。
  3. 锂电池自动充电管理。
- GD32主控系统：采用的单片机型号为GD32F103C8T6
  1. 通过I2C总线读取SHT30传感器的数据。
  2. 通过I2C总线控制OLED显示。
  3. 通过串口与wifi模块通信，包含了复位，晶振，SWD接口等电路。

- 部分芯片、模块选型介绍

  SHT30：温湿度传感器，IIC通讯，为减少PCB本身的干扰，芯片周围不铺铜。

  OLED模块：4PIN，使用IIC通讯，供电电压可使用5V或3.3V。

  wifi模块：WB3S WiFi&BLE 双协议模组，采用串口与GD32进行通讯。

  

------

### 软件程序介绍

- 涂鸦wifi模块参考文档
  1. 官方文档教程请看[文档教程](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/mcu-development-access/wifi-mcu-sdk-solution/overview-of-migrating-tuyas-mcu-sdk?id=K9hhi0xr5vll9)
  2. 官方视频教程请看[MCU SDK 移植](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/mcu-development-access/wifi-mcu-sdk-solution/overview-of-migrating-tuyas-mcu-sdk?id=K9hhi0xr5vll9)



### 总结

从硬件方面上讲。第一次完全自主地进行电路图原理图绘制、PCB设计、PCB打样、元器件选购、元器件焊接。做了很多以前想做但一直都没做的事情。还是挺感慨的。做成自己不会的果然就会很有成就感，也是感觉自己成长最快的时候。比如SHT30芯片的焊接着实费了好大的功夫才搞定。

也是第一次使用GD32芯片，因为ST芯片的爆缺爆涨，也是大家纷纷寻找国产芯片替换的时机。趁机也了解了一把GD32、AT32、HK32等芯片。GD32的库感觉和ST库很相似，但也不得不吐槽一把，标准库里的Keil例程居然不能更换芯片，好一阵拆腾才把裸机程序给跑起来。I2C硬件的标准库函数很不友好，感觉比不上IO模拟I2C好用，也或许是没有掌握了其中精髓吧。

总之，这次训练营对自己帮助很大。做了很多第一次的突破，虽然还有些遗憾。但总的感觉学到了很多东西。