/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#define OPTION_COUNT		3

static prev_menu_s prevMenuData;

static void mSelect(void);
static void itemLoader(byte);
static void setVolumeUI(void);
static void setVolumeAlarm(void);
static void setVolumeHour(void);
static inline byte setVolume(byte);
static void setMenuOptions(void);
static display_t thisdraw(void);
//	static int volUI=0; 
//	static int volAlarm=0; 
//	static int volHour=0; 



void mSoundOpen()
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_SOUNDMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);

//	setMenuOptions();
	
	menuData.func.draw = thisdraw; //绑定菜单画图函数，不知道退出时，这个函数并没有继续执行的原因

	setPrevMenuOpen(&prevMenuData, mSoundOpen);

	beginAnimation2(NULL);
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)

	appconfig_save();

	setPrevMenuExit(&prevMenuData);

	doAction(isExiting);
}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOptions();
	addBackOption();
}

static void setVolumeUI()
{
//   volUI = setVolume(volUI);

	appConfig.volUI = setVolume(appConfig.volUI);
	//画出来了但是又被擦掉了，一直在画图
//	char name[3];
//	sprintf(name,"%d",appConfig.volUI);
//	draw_string((char*)name, false, 64, FRAME_HEIGHT - 8);
}

static void setVolumeAlarm()
{
//	volAlarm = setVolume(volAlarm);

	appConfig.volAlarm = setVolume(appConfig.volAlarm);
	
}

static void setVolumeHour()
{
//	volHour = setVolume(volHour);

	appConfig.volHour = setVolume(appConfig.volHour);
}

static  byte setVolume(byte vol)
{
	vol++;
	if(vol > 3)
		vol = 0;
	return vol;
}

static display_t thisdraw()
{
	char name[3];
	
	switch(menuData.selected)
	{
		case 0:sprintf(name,"%d",appConfig.volUI);break;
		case 1:sprintf(name,"%d",appConfig.volAlarm);break;
		case 2:sprintf(name,"%d",appConfig.volHour);break;
	}
	
	draw_string((char*)name, false, 122, 0);
	return DISPLAY_DONE;
}

static void setMenuOptions()
{
//	setMenuOption_P(0, PSTR(STR_UI), menu_volume[volUI], setVolumeUI);
//	setMenuOption_P(1, PSTR(STR_ALARMS), menu_volume[volAlarm], setVolumeAlarm);
//	setMenuOption_P(2, PSTR(STR_HOURBEEPS), menu_volume[volHour], setVolumeHour);
	
	
	setMenuOption_P(0, PSTR(STR_UI), menu_volume[appConfig.volUI], setVolumeUI);
	setMenuOption_P(1, PSTR(STR_ALARMS), menu_volume[appConfig.volAlarm], setVolumeAlarm);
	setMenuOption_P(2, PSTR(STR_HOURBEEPS), menu_volume[appConfig.volHour], setVolumeHour);
}
