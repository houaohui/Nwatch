#include  "wifi_task.h"
#include "common.h"
#include "delay.h"



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
	if(cmd_finish==3)
	{
		console_log(1,"time&date:");
		Usart2_RxCounter=0;
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);
		WiFi_send("GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n");
		buttons_setFunc(BTN_1,down_line);
		buttons_setFunc(BTN_3,up_line); 
		
	}
	else if(cmd_finish==4)
	{
		console_log(1,"weather:");
		Usart2_RxCounter=0;
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);
		WiFi_send("GET https://api.seniverse.com/v3/weather/now.json?key=SIN2EqCTJrkaqqOpk&location=jiaozuo&language=en&unit=c\r\n");
		buttons_setFunc(BTN_1,down_line);
		buttons_setFunc(BTN_3,up_line); 
	}
	else
	{
		console_log(1,"->");
	}
	
	skip_aim_flag=1;
	return true;
}

void wifi_test_open(void)
{
	display_setDrawFunc(draw);
	buttons_setFuncs(btndown, btnExit, btnup );
	beginAnimation2(NULL);
	if(keep_on==0)  //第一次进入时打印此句话
		console_log(1,"WiFi TEST:");
	keep_on = true;
	Usart2_RxCompleted=0;
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
	console_loop_show();
	

	return DISPLAY_BUSY;
}




