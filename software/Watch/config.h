/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

// Fuses for ATmega328/328P
// Internal RC 8MHz 6 CK/14CK + 4.1ms startup
// Serial program (SPI) enabled
// Brown-out at 1.8V
// High:		0xDF
// Low:			0xD2
// Extended:	0xFE

#ifndef CONFIG_H_
#define CONFIG_H_

//#define CPU_DIV clock_div_1

// Hardware version
// 1 = PCB 1.0 - 1.1
// 2 = PCB 1.2
// 3 = PCB 1.3 - 1.4
#define HW_VERSION	3

#define USER_NAME "HouAohui"

// Firmware version
#define FW_VERSION	"  2021/5/29" // maybe use some __DATE__ __TIME__ stuff?

// Language
// 0 = English
// 1 = German (not done)
// 2 = Russian
#define LANGUAGE 0


// ����ѡ��
#define COMPILE_GAME1      1 // ��ϷBreakout    
#define COMPILE_GAME2      1 // ��ϷCar dodge
#define COMPILE_GAME3      0 // ��ϷFlappy thing (not finished) ��bug  δ��ɣ�ʵ��������
#define COMPILE_ANIMATIONS 1 //����
#define COMPILE_STOPWATCH  1 //���
#define COMPILE_TORCH      1 //�ֵ�Ͳ
#define COMPILE_TUNEMAKER  1 //3D����


//���������������Դ��ڴ�ӡ������Ϣ
#define COMPILE_UART       1
#define DEBUG_MSGS		     0
#define UART_BAUD          115200



//#define RTC_SRC   ///�����ⲿʱ��ʱ�� ��ע������          ������BUG ����Ҫ����޸�millis.c�����z�жϼ�ʱ���


#endif /* CONFIG_H_ */
