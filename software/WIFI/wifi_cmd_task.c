#include "wifi_cmd_task.h"
#include "common.h"
#include "TXT_Viewer.h"

#define STR_WIFICMDMENU  "WiFi CMD"
#define CMD1_NAME "deep sleep"
#define CMD2_NAME "light sleep"
#define CMD3_NAME "updata time&date"
#define CMD4_NAME "updata weather"
#define CMD5_NAME "reset wifi"

#define OPTION_COUNT 9

typedef void (*_cmd)(void);


static _cmd ondoing;
unsigned char cmd_finish=1;


static void mSelect(void);

static void itemLoader(byte num);

extern char CMD_S[10];
//等待动画执行命令
char skip_aim_flag=0;

static void cmd_start(_cmd wifi_cmd)
{
	CMD_S[0]=2;
	CMD_S[1]=0;
	CMD_S[2]=0;
	CMD_S[3]=0;
	CMD_S[4]=0;
	CMD_S[5]=0;
	CMD_S[6]=0;
	CMD_S[7]=0;
	cmd_finish=0;
	skip_aim_flag=1;
	ondoing = wifi_cmd;
}


void WiFi_CMD_updata(void)
{
	static millis8_t lastUpdate;
	static unsigned short time=200;
	
	  // Update every 20ms
	millis8_t now = millis();
	
	if(skip_aim_flag)
	{
		skip_aim_flag=0;
		lastUpdate = now;  //重新开始计时，
		time =800;  //等待800等动画完成
	}
	
	if ((millis8_t)(now - lastUpdate) >= time)
	{
		lastUpdate = now;
		time=200;   //200ms定时任务
		
		if(!cmd_finish)
		{
			ondoing();
		}
		  
	
	}
	
}

void wifi_comd_open(void)
{
	menuData.isOpen = true;  //打开菜单
	
	display_setDrawFunc(menu_draw);  //绑定绘制函数为menu_draw

	buttons_setFuncs(menu_up, menu_select, menu_down);  //绑定按键功能函数

	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_WIFICMDMENU));   //获取当前菜单信息（选项个数，菜单类型是文字还是图标）
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //绑定菜单的函数,如前进后退选择确认
	beginAnimation2(NULL);      
}



static void mSelect()
{
	doAction(false);  //执行指令
	menuData.isOpen = false;  //关闭菜单
	beginAnimation(wifi_test_open);  //返回状态输出界面,观看指令执行情况
}




void cmd1(void)
{
	cmd_start(deep_sleep);
}

void cmd2(void)
{
	cmd_start(light_sleep);
}



void cmd3(void)
{
	
	cmd_start(connect_my_phone);
}


void cmd4(void)
{
	cmd_start(get_weather_cmd); 
}

void cmd5(void)
{
	cmd_start(reset_wifi_cmd); 
}

void cmd6(void)
{
	cmd_start(scan_wifi); 
}


void cmd7(void)
{
	cmd_start(smartconfig); 
}

void cmd8(void)
{
	cmd_start(wifi_status); 
}

void txt(void)
{
	
	cmd_start(txt_test_open); 
}

static void itemLoader(byte num)
{
	num = 0;

	setMenuOption_P(num++, PSTR(CMD1_NAME), NULL, cmd1);

	setMenuOption_P(num++, PSTR(CMD2_NAME), NULL, cmd2);

	setMenuOption_P(num++, PSTR(CMD3_NAME), NULL, cmd3);
	
	setMenuOption_P(num++, PSTR(CMD4_NAME), NULL, cmd4);
	
	setMenuOption_P(num++, PSTR(CMD5_NAME), NULL, cmd5);
	
	setMenuOption_P(num++, PSTR("scan wifi"), NULL, cmd6);
	
	setMenuOption_P(num++, PSTR("wifi status"), NULL, cmd8);
	
	setMenuOption_P(num++, PSTR("smartconfig"), NULL, cmd7);
	
	setMenuOption_P(num++, PSTR("TXT viewer test"), NULL, txt);

	addBackOption();
}



