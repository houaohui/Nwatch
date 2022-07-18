/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ����Wifi���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	       //������Ҫ��ͷ�ļ�
#include "sys.h"
#include "gui_log_console.h"
#include "wait_data_clock.h"

extern char TCP_flag;      //0����ʼ��UDPʧ��  1����ʼ��UDP�ɹ�
extern char  Data_buff[1024];
extern char RX_DAT_OK;

#define RESET_IO(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_log        printf
#define WiFi_send       u2_printf           //����2���� WiFi
#define WiFi_RxCounter    Usart2_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       Usart2_RxBuff       //����2���� WiFi
#define WiFi_RXBUFF_SIZE  USART2_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "16s"                     //·����SSID����
#define PASS   "87654321"                 //·��������

char WiFi_Init(void);
void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_GetIP(u16 timeout);
char WiFi_Get_LinkSta(void);
char WiFi_Get_Data(char *data, char *len, char *id);
char WiFi_SendData(char id, char *databuff, int data_len, int timeout);
char WiFi_Connect_TCP(int timeout);
char WiFi_Connect_XIN_ZHI_TCP_Server(void);
//////////////////
void deep_sleep(void);
void light_sleep(void);
void connect_my_phone(void);
void reset_wifi_cmd(void);
void get_weather_cmd(void);

void show_wather_data(void);

void show_time_data(void);
void updata_time_data(void);

void scan_wifi(void);
void smartconfig(void);


void wifi_status(void);
#endif


