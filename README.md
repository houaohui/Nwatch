# N-watch--可联网版

视频介绍：https://www.bilibili.com/video/BV1di4y1o7op?share_source=copy_web

## 0.关于本项目

本项目的制作来源于作者的兴趣爱好，希望做出一个美观功能强大的可编程手表，在此固件的基础上可根据自己的需求开发扩展出不同功能，硬件软件不同程度的借鉴了已经开源的项目

此项目程序框架来源于N-watch，适配到了stm32这款主控上，相应的原程序具有的手表基本功能本程序也有，本程序在原程序基础上添加了类似命令行滚动输出信息的功能，与ESP01F模块通信联网，一个可以伸缩变换的波形显示器。还有一些其他功能并没有实现，陀螺仪因为数据错误没有应用，电量显示没有适配，因为硬件忘了留出引脚了（尴尬）。。

硬件外形借鉴了一位B站up主的作品i-watch的外形，其外形简洁美观，非常喜欢。手表采用两层板堆叠设计，分为主板和屏幕板，板子上面均有元件焊接，屏幕板可以认为就是换了外形的SPI接口的1.3寸OLED模块屏幕。两层堆叠设计可以充分利用空间使得手表厚度有效减小，两层板之间使用普通排针连接，除去电池厚度，手表厚度仅为一个屏幕厚度+两个PCB板厚度+一个type-B接口厚度

## 1.主要硬件配置：

主控：stm32c8t6

WIFI：ESP01F

锂电池充电芯片：XT4054K421MR-G

3V3电源：TPS78233DDCT

加速度计： MPU6050

OLED：1.3寸SSD1315

## 2.主要软件功能：

时间日期显示，秒表，闹钟，模拟手电筒，音乐播放

三个游戏（打砖块，赛车，happybird）

设置时间日期，休眠时间

设置UI 闹钟 整点音量 

设置显示亮度 显示方向 显示色彩翻转 移除动画 刷新率 背景灯开关

联网获取时间日期天气等信息

波形显示器，显示帧率

## 3.硬件打样说明

AD工程中PCB文件中Q1mos管的PROJ和GND两引脚与原理图文件中的两个引脚是相反的，因为偷懒用的三极管的封装，要改工程需要注意一下，不想改动可以直接打板发厂

## 4.固件下载说明

固件只能使用ST-link下载，因为板子芯片上的boot引脚都接地，没有引出，无法使用串口下载

## 5.其他

**主要元件购买链接：**

3225贴片无源晶振8M https://m.tb.cn/h.fQ80XDV?sm=8d49a0 

TPS78233DDC https://m.tb.cn/h.fQ81LV5?sm=0da310 

手表带 https://m.tb.cn/h.fki3fVY?sm=a50620 

MOS 管/N沟道 https://m.tb.cn/h.f9hAn7K?sm=6317dc 

3.7v锂电池 https://m.tb.cn/h.f9hzHYn?sm=4c17b3 

1.29寸OLED（ssd1315）  https://m.tb.cn/h.fQ8Y99V?sm=67f3e6 

贴片三极管 https://m.tb.cn/h.fkiWWky?sm=f0e78a 

侧拨滑动开关 https://m.tb.cn/h.fQ81uT5?sm=d290a1 

XT4054K421MR锂电池充电芯片 https://m.tb.cn/h.fki3CNS?sm=a629e4 

轻触开关 https://m.tb.cn/h.fkie8MZ?sm=1c3dd5 

ESP-01F https://m.tb.cn/h.fkiVCPh?sm=8cef48 

无源贴片蜂鸣器 https://m.tb.cn/h.fkieHla?sm=01d911 





