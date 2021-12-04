#include "lcd.h"
#include "stdlib.h"
#include "usart.h"
#include "delay.h"	 
#include "draw_api.h"
#include "util.h"
#include "common.h"


u8	oledBuffer[FRAME_BUFFER_SIZE];			    

//初始化lcd
//该初始化函数可以初始化各种ALIENTEK出品的LCD液晶屏
//本函数占用较大flash,用户可以根据自己的实际情况,删掉未用到的LCD初始化代码.以节省空间.
void LCD_Init(void)
{ 
	InitGraph();

    if(!appConfig.display180)
	{
		WriteCmd(0xA1); 
		WriteCmd(0XC8); 
	}
	else
	{
		WriteCmd(0xA0); 
		WriteCmd(0xC0); 
	}
}
  
//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	ClearScreen();
}  



void oled_flush(void)
{
	
	//OLED_FILL(oledBuffer);
	u8 i,j;
	unsigned char *p;
	p=oledBuffer;

	for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			if(appConfig.invert)
			{
				WriteDat(~(*p++));
			}
			else
			{
				WriteDat(*p++);
			}
		}
	}
}




















