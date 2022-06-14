#include  "wifi_task.h"
#include "common.h"
#include "delay.h"
#include "gui_log_console.h"



extern bool keep_on;

display_t draw(void);





static bool btnExit()
{
	animation_start(display_load, ANIM_MOVE_OFF);
	keep_on = false;
	return true;
}


static bool btnup()
{
	
	animation_start(wifi_comd_open, ANIM_MOVE_OFF);
	return true;
}

extern unsigned char cmd_finish;

extern char skip_aim_flag;

static bool btndown()
{
	
	buttons_setFunc(BTN_1,down_line);
	buttons_setFunc(BTN_3,up_line); 
	

	console_log(1,"->");

	
	skip_aim_flag=1;
	return true;
}
extern char LOOP_SHOW_MD;
extern short pix_show_idx;
extern char NEW_LINE_AMI;
char log_first=0;

void wifi_test_open(void)
{
	display_setDrawFunc(draw);
	buttons_setFuncs(btndown, btnExit, btnup );
	beginAnimation2(NULL);

	//loop show模式
	LOOP_SHOW_MD=true;
	Usart2_RxCompleted=0;
	
	//清除显示偏移
	//pix_show_idx=64;
	NEW_LINE_AMI=true;
	
	//第一次进入时打印此句话
	if(keep_on==0)
		log_first=true;
	
	keep_on = true;
}

//跳过动画打印
short time=50;
char skipped_AMI(bool with_conditions)
{
	if(log_first==true&&with_conditions)
	{
		if(--time==0)
			log_first=false;
		return false;
	}
	else if(log_first==false&&!time)
	{
		time=50;
		return true;
	}
	return false;
}


static display_t draw()
{
	if(!cmd_finish)
	{
		WiFi_CMD_updata();
	}
	else
	{
		if(Usart2_RxCompleted == 1&&!buzzer_buzzing())
		{
			Usart2_RxCompleted=0;
			console_log(1,"%s",&Data_buff[2]);
			console_log(1,"data_size: %d",(Data_buff[0]*256+Data_buff[1])); //串口输出信息
			
			if(cmd_finish==3)
			{
				updata_time_data();
			}
			
			if(cmd_finish==4)
			{
				show_wather_data();
			}
		}
		
	}
	
	if(skipped_AMI(!is_loging()))
	{
		console_log(1,"WiFi TEST:");
	}
	
	console_loop_show();
	

	return DISPLAY_BUSY;
}




