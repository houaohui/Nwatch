/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"
#include "lcd.h"
#include "led.h"

#define OPTION_COUNT	6

static prev_menu_s prevMenuData;

static void mSelect(void);
static void itemLoader(byte);
static void setBrightness(void);
static void setInvert(void);
static void setRotate(void);
#if COMPILE_ANIMATIONS
static void setAnimations(void);
#endif
static void setMenuOptions(void);

static display_t thisDraw(void);

void mDisplayOpen()
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_DISPLAYMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);

	//setMenuOptions();//找到动画移除bug了
	menuData.func.draw = thisDraw;
	setPrevMenuOpen(&prevMenuData, mDisplayOpen);

	beginAnimation2(NULL);
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
		appconfig_save();
	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());
	
	
}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOptions();
	addBackOption();
}
#include "draw_api.h"
static void setBrightness()
{
 	byte brightness = appConfig.brightness;
	brightness++;
	if(brightness > 3)
		brightness = 1;
		
	appConfig.brightness = brightness;

	WriteCmd(0x81); //--set contrast control register
	WriteCmd(brightness*85); //亮度调节 0x00~0xff
 
}

static void setInvert()
{
	bool invert = !appConfig.invert;
	appConfig.invert = invert;

//	oled_setInvert(invert);
}

static void setRotate() 
{
	bool rotate = !appConfig.display180;
	appConfig.display180 = rotate;

//	oled_set180(rotate);
	
	if(rotate)
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

static void setLEDs() {
  appConfig.CTRL_LEDs = !appConfig.CTRL_LEDs;
}

#if COMPILE_ANIMATIONS
static void setAnimations()
{
	bool anims = !appConfig.animations;
	appConfig.animations = anims;
}


extern byte MY_FPS;

static void setFPS()
{
	byte fps = MY_FPS/2;
	fps++;
	if(fps > 30)
		fps = 10;
	MY_FPS = fps*2;
}

static display_t thisDraw()
{
	if(menuData.selected == 4)
	{
		char buff[4];
		sprintf_P(buff, PSTR("%hhuS"), (unsigned char)MY_FPS);
		draw_string(buff, NOINVERT, 56, 40);
	}
	return DISPLAY_DONE;
}

#endif

static void setMenuOptions()
{
//	setMenuOption_P(0, PSTR(STR_BRIGHTNESS), menu_brightness[appConfig.brightness], setBrightness);
//	setMenuOption_P(1, PSTR(STR_INVERT), menu_invert, setInvert);
//	setMenuOption_P(2, PSTR(STR_ROTATE), menu_rotate, setRotate);
//#if COMPILE_ANIMATIONS
//	setMenuOption_P(3, PSTR(STR_ANIMATIONS), menu_anim[appConfig.animations], setAnimations);
//#endif
//  setMenuOption_P(4, PSTR(STR_LEDS), menu_LEDs[appConfig.CTRL_LEDs], setLEDs);
//	

	
	setMenuOption_P(0, PSTR(STR_BRIGHTNESS), menu_brightness[appConfig.brightness], setBrightness);
	setMenuOption_P(1, PSTR(STR_INVERT), menu_invert, setInvert);
	setMenuOption_P(2, PSTR(STR_ROTATE), menu_rotate, setRotate);
#if COMPILE_ANIMATIONS
	setMenuOption_P(3, PSTR(STR_ANIMATIONS), menu_anim[appConfig.animations], setAnimations);
	setMenuOption_P(4, PSTR(STR_SETFPS), menu_setfps, setFPS);
#endif
  setMenuOption_P(5, PSTR(STR_LEDS), menu_LEDs[appConfig.CTRL_LEDs], setLEDs);
	

}
