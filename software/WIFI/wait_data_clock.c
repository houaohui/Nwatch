/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            实现定时器4功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/
#include "wait_data_clock.h"
#include "stm32f10x.h"  //包含需要的头文件
#include "wifi.h"	       //包含需要的头文件
#include "usart2.h"

#define  WiFi_DATA_TIME_OK   30


char is_wifi_data_start=0;  //是否开始接收数据

unsigned char wifi_time_cnt=0;


//服务函数放在定时器中断中
void wifi_wait_data_hander(void)
{
	if(is_wifi_data_start)
	{
		wifi_time_cnt++;
		if(wifi_time_cnt > WiFi_DATA_TIME_OK)
		{
			Usart2_RxCompleted = 1;                                       //串口2接收完成标志位置位
			memcpy(&Data_buff[2],Usart2_RxBuff,Usart2_RxCounter);         //拷贝数据
			Data_buff[0] = WiFi_RxCounter/256;                            //记录接收的数据量		
			Data_buff[1] = WiFi_RxCounter%256;                            //记录接收的数据量
			Data_buff[WiFi_RxCounter+2] = '\0';                           //加入结束符
			WiFi_RxCounter=0;                                             //清零计数值
			is_wifi_data_start=0;                        				  //关闭接收器
			wifi_time_cnt=0;
		}
	}

}



