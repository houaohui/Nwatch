#include "lcd.h"
#include "stdlib.h"
#include "usart.h"
#include "delay.h"	 
#include "draw_api.h"
#include "util.h"
#include "common.h"


u8	oledBuffer[FRAME_BUFFER_SIZE];			    

//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ALIENTEK��Ʒ��LCDҺ����
//������ռ�ýϴ�flash,�û����Ը����Լ���ʵ�����,ɾ��δ�õ���LCD��ʼ������.�Խ�ʡ�ռ�.
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
	
	//WriteCmd(0xa7); //--set normal display
}
  
//��������
//color:Ҫ���������ɫ
//void LCD_Clear(u16 color)
//{
//	ClearScreen();
//}  



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




















