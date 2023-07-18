# Transplanted LGVL on FireBeetle 2 ESP32-E

## ⚙️ 依赖

1. [FireBeetle Board ESP32 E](https://wiki.dfrobot.com.cn/_SKU_DFR0654_FireBeetle_Board_ESP32_E)，参看官网帮助页面下载库
2. TFT LCD屏幕，硬件SPI连接
3. [SHT40传感器](https://www.mikroe.com/temphum-15-click)，I2C通信，使用[Github-adafruit/Adafruit_SHT4X](https://github.com/adafruit/Adafruit_SHT4X)库，可通过Arduino直接下载
4. [VL53L0激光测距传感器](https://wiki.dfrobot.com.cn/_SKU_SEN0245_VL53L0_Distance_Ranging_Sensor)，I2C通信，参见官网获取有关库，可通过Arduino直接下载
5. 所用到的库文件在 `./libraries` 目录下

**👉 本工程基于VSCode与Arduino开发，在此环境下，工程可直接编译通过**

## 🛠 工程

**工程代码讲解及效果参见：**

1. [CSDN工程详解：基于FireBeetle 2 ESP32-E开发板的LVGL移植及传感器显示(Arduino+TFT_eSPI+LVGL)](https://blog.csdn.net/weixin_46422143/article/details/128507489)
2. [B站：工程演示](https://www.bilibili.com/video/BV1uG4y1m7MB/)
