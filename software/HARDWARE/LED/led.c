#include "led.h" 
#include "common.h"

/*
   Project: N|Watch
   Author: Zak Kemble, contact@zakkemble.co.uk
   Copyright: (C) 2013 by Zak Kemble
   License: GNU GPL v3 (see License.txt)
   Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
*/

// LED control

typedef struct {
  byte flashLen;			// How long to light up for
  millis8_t startTime;	//
} led_s;

static led_s ledRed;
static led_s ledGreen;


//?????PF9??PF10??????.?????????????????		    
//LED IO?????
void led_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);						 //PA.8 输出高
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//PA4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15
}


static void flash(led_s* led, byte len,byte brightness)
{
  led->flashLen = len;
  led->startTime = millis();

}

bool CTRL_LEDs=0;
void led_flash(led_t led, byte len, byte brightness)
{
	
	//if(!appConfig.CTRL_LEDs)//结构体中的这个bool变量始终是0，在led设置里面能改，但是在这里又变为零了？？
	if (!CTRL_LEDs) //必须这样操作才可以???
	{

		switch (led)
		{
		  case LED_RED:
				flash(&ledRed, len, brightness);LED0=!brightness;
			break;
		  case LED_GREEN:
				flash(&ledGreen, len, brightness);LED1=!brightness;
			break;
		  default:
			break;
		}

		pwrmgr_setState(PWR_ACTIVE_LED, PWR_STATE_IDLE);
	}
}

// Is an LED on?
BOOL led_flashing()
{
  return ledRed.flashLen || ledGreen.flashLen;
}

static BOOL _update(led_s* led,  byte pin)        //(volatile unsigned long  *)
{
	if (led->flashLen && (millis8_t)(millis() - led->startTime) >= led->flashLen)
	{
		led->flashLen = 0;
	}
	

	return led->flashLen;
}

void led_update()
{

	
	BOOL red = _update(&ledRed,LED_RED);          //LED0 PFout(9)
	BOOL green = _update(&ledGreen, LED_GREEN);    //LED1 PFout(10)

	
	if (!red && !green)
	{
		LED0=1;//关闭
		LED1=1;
		pwrmgr_setState(PWR_ACTIVE_LED, PWR_STATE_NONE);
	}
}

// Turn off LEDs
void led_stop()
{
  led_flash(LED_GREEN, 0, 0);
  led_flash(LED_RED, 0, 0);
  led_update();
}



