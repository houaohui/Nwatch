/*
   Project: N|Watch
   Author: Zak Kemble, contact@zakkemble.co.uk
   Copyright: (C) 2013 by Zak Kemble
   License: GNU GPL v3 (see License.txt)
   Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
*/
#include "buttons.h"
#include "common.h"
#include "sys.h"
#include "led.h"
#include "yaogan.h"


#define BTN_IS_PRESSED	4
#define BTN_NOT_PRESSED	4





typedef struct {
  millis_t pressedTime;	// Time of press
  bool processed;			// Time of press has been stored (don't store again until next press)
  byte counter;			// Debounce counter
  bool funcDone;			// Function has been ran (don't run again until next press)
  button_f onPress;		// Function to run when pressed
  const ulong* tune;		// Tune to play when pressed
} s_button;

static s_button buttons[BTN_COUNT];

static void processButtons(void);
static void processButton(s_button*, BOOL);
static byte bitCount(byte);

//EMPTY_INTERRUPT(PCINT0_vect);

void buttons_init()
{
  buttons_startup();

	//�������Ĳ�������
  // Assign tunes
  buttons[BTN_1].tune = tuneBtn1;
  buttons[BTN_2].tune = tuneBtn2;
  buttons[BTN_3].tune = tuneBtn3;

  // Set up interrupts
//#ifdef __AVR_ATmega32U4__
//  SET_BITS(PCMSK0, PCINT4, PCINT6, PCINT7);
//#else
//  SET_BITS(PCMSK1, PCINT9, PCINT10, PCINT11);
//#endif
//  BTN_INT_ON();
}

void buttons_update()
{
  static millis8_t lastUpdate;

  // Update every 10ms
  millis8_t now = millis();
  if ((millis8_t)(now - lastUpdate) >= 10)
  {
	 // printf("1");
    lastUpdate = now;
    processButtons();
  }
}

//������ʼ������
// Sets button pins to INPUT with PULLUP
void buttons_startup()
{
	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
// 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4; //KEY0 KEY1 KEY2��Ӧ����
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
//	
//	 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP��Ӧ����PA0
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTCʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PA4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;//PC5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOC5
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA.0
}

// Sets button pins to OUTPUT LOW
// This stops them wasting current through the pull-up resistor when pressed
void buttons_shutdown()
{
	//�������sleep����������رհ�����������
	
//  pinPullup(BTN_1_P,	PU_DIS);
//  pinPullup(BTN_2_P,	PU_DIS);
//  pinPullup(BTN_3_P,	PU_DIS);

//  pinMode(BTN_1_P,	OUTPUT);
//  pinMode(BTN_2_P,	OUTPUT);
//  pinMode(BTN_3_P,	OUTPUT);
}

static void processButtons()
{
  // Get button pressed states
  BOOL isPressed[BTN_COUNT];
  isPressed[BTN_1] = KEY0;  //rightһ�����¼���
  isPressed[BTN_2] = KEY1;
  isPressed[BTN_3] = KEY2;  //left

  // Process each button
	
  LOOPR(BTN_COUNT, i)
  processButton(&buttons[i], !isPressed[i]);
}

static void processButton(s_button* button, BOOL isPressed)
{
  button->counter <<= 1;
  if (isPressed)
  {
    // Set debounce counter bit
    button->counter |= 1;

    // Are enough bits set to count as pressed?//�൱�ڰ�������
    if (bitCount(button->counter) >= BTN_IS_PRESSED)
    {
      // Store time of press
      if (!button->processed)
      {
        button->pressedTime = millis();
        button->processed = true;
      }

      // Run function
      if (!button->funcDone && button->onPress != NULL && button->onPress())
      {
        button->funcDone = true;
        tune_play(button->tune, VOL_UI, PRIO_UI);

        led_flash(LED_GREEN, LED_FLASH_FAST, LED_BRIGHTNESS_MAX);
        
      }
    }
  }
  else // Not pressed
  {
    // Has button been not pressed for long enough?
    if (bitCount(button->counter) <= BTN_NOT_PRESSED)
    {
      button->processed = false;
      button->funcDone = false;
    }
  }
}

// Count set bits in value
static byte bitCount(byte val)
{
  byte count = 0;
  for (; val; val >>= 1)
    count += val & 1;
  return count;
}

//����һ�����غ���ָ��ĺ���
// Set new function to run when button is pressed and return the old function
button_f buttons_setFunc(btn_t btn, button_f func)
{
  button_f old = buttons[btn].onPress;
  buttons[btn].onPress = func;
  return old;
}

// Set functions to run for each button
void buttons_setFuncs(button_f btn1, button_f btn2, button_f btn3)
{
  buttons[BTN_1].onPress = btn1;
  buttons[BTN_2].onPress = btn2;
  buttons[BTN_3].onPress = btn3;
}

/*
  // Get how long a button has been pressed for
  millis_t buttons_pressTime(btn_t btn) // set max press time to 1 min!!!
  {
	s_button* button = &buttons[btn];
	if(button->pressed == BTN_NOT_PRESSED)
		return 0;
	return (millis() - button->pressedTime);
  }
*/

// See if a button has been pressed in the past x milliseconds
bool buttons_isActive()
{
//  // If sleep has been disabled then just say that the buttons are always active
  if (!appConfig.sleepTimeout)
    return true;

//  // Get timeout val in ms
  uint timeout = (appConfig.sleepTimeout * 5) * 1000;
//  uint timeout =  1000;

  // See if a button has been pressed within that timeout
  LOOPR(BTN_COUNT, i)
  {
    if (millis() - buttons[i].pressedTime < timeout)
      return true;
  }

  return false;
}

// Set button status to pressed, processed etc but don't run their functions
void buttons_wake()
{
  LOOPR(BTN_COUNT, i)
  {
    buttons[i].funcDone		= true;
    buttons[i].processed	= true;
    buttons[i].counter		= BTN_IS_PRESSED;
    buttons[i].pressedTime	= millis();
  }
}
