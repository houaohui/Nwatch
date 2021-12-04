/*
   Project: N|Watch
   Author: Zak Kemble, contact@zakkemble.co.uk
   Copyright: (C) 2013 by Zak Kemble
   License: GNU GPL v3 (see License.txt)
   Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
*/

#include "common.h"
#include "lcd.h"
#include  "led.h"
#if COMPILE_TORCH

static uint strobe;

static bool btnExit(void);
static bool btnFlashRate(void);
static display_t draw(void);

void torch_open()
{
	//menu_close();  里面有打开动画，需要在下面再添加一下

	strobe = 0;
	display_setDrawFunc(draw);
	buttons_setFuncs(btnExit, btnFlashRate, btnExit);
	beginAnimation2(NULL); //打开动画动画过度//因为torch没有画位图函数所以动画功能没有作用
}

static bool btnExit()
{
//display_load();  //返回watch face
	//animation_start(display_load, ANIM_MOVE_OFF);   
	//因为torch没有画位图函数所以动画功能没有作用
	animation_start(back, ANIM_MOVE_OFF);  //带关闭动画的退出
	return true;
}

static bool btnFlashRate()
{
  if (strobe < 500)
    strobe += 50 * ((strobe / 50) + 1);
  else
    strobe = 0;
  return true;
}

static display_t draw()
{
  static bool invert;
  static millis_t lastStrobe;

  if (strobe)
  {
    millis_t now = millis();
    if (now - lastStrobe >= strobe)
    {
		lastStrobe = now;
		invert = !invert;
		memset(&oledBuffer, ~(invert?0xff:0x00), FRAME_BUFFER_SIZE);
//      oled_setInvert(invert);
			
       //led_flash(invert ? LED_GREEN : LED_RED, 20, 255);
    }
    return DISPLAY_BUSY;
  }
  
	memset(&oledBuffer, 0xff, FRAME_BUFFER_SIZE);
	
//  oled_setInvert(true);
//    led_flash(LED_GREEN, 100, 255);
//    led_flash(LED_RED, 100, 255);
  
  return DISPLAY_DONE;
}

#endif
