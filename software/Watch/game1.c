/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#if COMPILE_GAME1

#define PLATFORM_WIDTH	12    //ƽ̨���
#define PLATFORM_HEIGHT	4     //ƽ̨�߶�
#define UPT_MOVE_NONE	0     //���ƶ�
#define UPT_MOVE_RIGHT	1     //�����ƶ�
#define UPT_MOVE_LEFT	2     //�����ƶ�
#define BLOCK_COLS		32    //ש������
#define BLOCK_ROWS		5     //ש������
#define BLOCK_COUNT		(BLOCK_COLS * BLOCK_ROWS)  //ש������

typedef struct{
	float x;
	float y;
	float velX;
	float velY;
}s_ball;

static const byte block[] PROGMEM ={
	0x07,0x07,0x07,
};

static const byte platform[] PROGMEM ={
	0x60,0x70,0x50,0x10,0x30,0xF0,0xF0,0x30,0x10,0x50,0x70,0x60,
};

static const byte ballImg[] PROGMEM ={
	0x03,0x03,
};

static bool btnExit(void);
static bool btnRight(void);
static bool btnLeft(void);
display_t game1_draw(void);

static byte uptMove;    //�ƶ�״̬
static s_ball ball;
static bool* blocks=NULL;
static byte lives;      //����ֵ
static uint score;      //�÷�
static byte platformX;  
static bool gameEnded;

extern bool keep_on;
//��Ϸ��ʼ
void game1_start()
{
	//menu_close();//�����д򿪶�����Ҫ�����һ��
	//menuData.isOpen = false;
	keep_on = true;
	beginAnimation2(NULL);
	srand(millis());
	display_setDrawFunc(game1_draw);
	buttons_setFuncs(btnRight, btnExit, btnLeft);
	
	uptMove = UPT_MOVE_NONE;

	ball.x = FRAME_WIDTH / 2;
	ball.y = FRAME_HEIGHT - 10;
	ball.velX = -0.5;
	ball.velY = -0.6;
//	ball.velX = -1;
//	ball.velY = -1.1;
	if(blocks==NULL||gameEnded)  //Ϊ�ջ�����Ϸ�������·����ڴ�
	{
		if(gameEnded)
		free(blocks);
		blocks = calloc(BLOCK_COUNT, 1);  //void* calloc��unsigned int num��unsigned int size���ڴ�Ķ�̬�洢���з���num������Ϊsize�������ռ�
	}
									//��������һ��ָ�������ʼ��ַ��ָ��,calloc�������Զ����ڴ��ʼ��Ϊ0
	lives = 3;
	score = 0;
	platformX = (FRAME_WIDTH / 2) - (PLATFORM_WIDTH / 2);    //ƽ̨X���꣨����ƽ̨��ʼ���꣩ = ��Ļ���/2��ȥƽ̨���/2
}

#define OPTION_COUNT 3
static prev_menu_s prevMenuData;
static void itemLoader(byte num);
static void mSelect(void);

static void game1_meun(void)
{
	//menuData.isOpen = true;  //�򿪲˵�
	setPrevMenuOpen(&prevMenuData, game1_meun);//��ν�������˵����ϼ��˵�Ҳ���ᱻ���ǣ��ϼ��˵�Ӧ������Ϸѡ��
	buttons_setFuncs(menu_up, menu_select, menu_down);
	display_setDrawFunc(menu_draw);  //�󶨻��ƺ���Ϊmenu_draw
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR("game setting"));   //��ȡ��ǰ�˵���Ϣ��ѡ��������˵����������ֻ���ͼ�꣩
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //�󶨲˵��ĺ���,��ǰ������ѡ��ȷ��    
	beginAnimation2(NULL);
}

//doActionִ�еĺ������ܴ������������˳������ᱻ����
static void mSelect(void)
{
	setPrevMenuExit(&prevMenuData);//�洢�ϴβ˵���ѡ��
	doAction(true);  //���˳�����ִ��ָ��
}
static char automode=0;

void resume(void)
{
	display_setDrawFunc(game1_draw);
	buttons_setFuncs(btnRight, btnExit, btnLeft);
	beginAnimation2(NULL);
}

void auto_mode(void)
{
	automode=~automode;
	display_setDrawFunc(game1_draw);
	buttons_setFuncs(btnRight, btnExit, btnLeft);
	beginAnimation2(NULL);
}

static void game_exit(void)
{
	free(blocks);    //�ͷ��ڴ�ռ�ĺ���
	keep_on = false;
	menuData.prevMenu = prevMenuData.last; // ǿ��ָ���ϼ��˵� 
	back();
}

static void itemLoader(byte num)
{
	num = 0;
	setMenuOption_P(num++, PSTR("resume"), NULL, resume);
	setMenuOption_P(num++, PSTR("auto mode"), NULL, auto_mode);
	setMenuOption_P(num++, PSTR("exit game"), NULL, game_exit);//���˳�ѡ��ȼۣ��������Ը����ֺ����һЩ����
	addBackOption();
}


//��Ϸ�˳�
static bool btnExit()
{
	free(blocks);    //�ͷ��ڴ�ռ�ĺ���
	if(lives == 255)  //����ֵ�������������Ϊ��
		game1_start();
	else
		animation_start(game1_meun, ANIM_MOVE_OFF);
	return true;
}
//�����ƶ�
static bool btnRight()
{
	uptMove = UPT_MOVE_RIGHT;
	return false;
}
//�����ƶ�
static bool btnLeft()
{
	uptMove = UPT_MOVE_LEFT;
	return false;
}
//��Ϸ��ͼ
display_t game1_draw()
{
	gameEnded = ((score >= BLOCK_COUNT) || (lives == 255));  //��Ϸ������־�����÷ִ���ש������������ֵ���

	byte platformXtmp = platformX;        //�������ƽ̨X����

	// Move platform
	if(uptMove == UPT_MOVE_RIGHT)    //�����ƶ�
		platformXtmp += 3;
	else if(uptMove == UPT_MOVE_LEFT)   //�����ƶ�
		platformXtmp -= 3;
	uptMove = UPT_MOVE_NONE;     //Ĭ�ϲ��ƶ�

	// Make sure platform stays on screen
	if(platformXtmp > 250)
		platformXtmp = 0;
	else if(platformXtmp > FRAME_WIDTH - PLATFORM_WIDTH)
		platformXtmp = FRAME_WIDTH - PLATFORM_WIDTH;
	
	if(automode)
	{
		platformXtmp = ball.x + ball.velX;
	}

	// Draw platform ����ƽ̨ͼ��
	draw_bitmap(platformXtmp, FRAME_HEIGHT - 8, platform, 12, 8, NOINVERT, 0);

	platformX = platformXtmp;

	// Move ball
	if(!gameEnded)   //�����Ϸû�н�����С������ƶ�
	{
		ball.x += ball.velX;
		ball.y += ball.velY;
	}

	bool blockCollide = false;   //��ײ��־
	const float ballX = ball.x;
	const byte ballY = ball.y;

	// Block collision    //����ײ
	byte idx = 0;
	LOOP(BLOCK_COLS, x)  //for(x=0;x<BLOCK_COLS;x++)
	{
		LOOP(BLOCK_ROWS, y)
		{                         //С����ש���λ�ù�ϵ�������
			if(!blocks[idx] && ballX >= x * 4 && ballX < (x * 4) + 4 && ballY >= (y * 4) + 8 && ballY < (y * 4) + 8 + 4)
			{
				buzzer_buzz(100, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);    //������
//				led_flash(LED_GREEN, 50, 255);    //����˸
				blocks[idx] = true;   //ש����ʧ
				blockCollide = true;  //��ײ��־λ��һ
				score++;    //�÷ּ�һ
			}
			idx++;
		}
	}

	// Side wall collision
	if(ballX > FRAME_WIDTH - 2)
	{
		if(ballX > 240)
			ball.x = 0;		
		else
			ball.x = FRAME_WIDTH - 2;
		ball.velX = -ball.velX;		
	}
	if(ballX < 0)
	{
		ball.x = 0;		
		ball.velX = -ball.velX;	
	}

	// Platform collision С����ƽ̨��ײ
	bool platformCollision = false;         //С���Y���������Ļ�߶ȼ�ȥƽ̨�߶�&&С��X������ƽ̨��ʼ�����ƽ̨��ʼ�������ƽ̨���֮��
	if(!gameEnded && ballY >= FRAME_HEIGHT - PLATFORM_HEIGHT && ballY < 240 && ballX >= platformX && ballX <= platformX + PLATFORM_WIDTH)
	{
		platformCollision = true;
		buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL);
		ball.y = FRAME_HEIGHT - PLATFORM_HEIGHT;
		if(ball.velY > 0)
			ball.velY = -ball.velY;
		ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0   X�����ٶ��������
	}

	// Top/bottom wall collision
	if(!gameEnded && !platformCollision && (ballY > FRAME_HEIGHT - 2 || blockCollide))
	{
		if(ballY > 240)
		{
			buzzer_buzz(200, TONE_2_5KHZ, VOL_UI, PRIO_UI, NULL);
			ball.y = 0;
		}
		else if(!blockCollide)
		{
			buzzer_buzz(200, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);
			ball.y = FRAME_HEIGHT - 1;
			lives--;
		}
		ball.velY *= -1;
	}

	// Draw ball  ����С��
	draw_bitmap(ball.x, ball.y, ballImg, 2, 8, NOINVERT, 0);

	// Draw blocks  ����ש��
	idx = 0;
	LOOP(BLOCK_COLS, x)
	{
		LOOP(BLOCK_ROWS, y)
		{
			if(!blocks[idx])
				draw_bitmap(x * 4, (y * 4) + 8, block, 3, 8, NOINVERT, 0);
			idx++;
		}
	}

	// Draw score   ��ӡ����
	char buff[6];
	sprintf_P(buff, PSTR("%u"), score);
	draw_string(buff, false, 0, 0);

	// Draw lives   ��ӡ����ֵ
	if(lives != 255)
	{
		LOOP(lives, i)
			draw_bitmap((FRAME_WIDTH - (3*8)) + (8*i), 1, livesImg, 7, 8, NOINVERT, 0);
	}	

	// Got all blocks   //��������ש��
	if(score >= BLOCK_COUNT)
		draw_string_P(PSTR(STR_WIN), false, 50, 32);  //������Ϸʤ��

	// No lives left (255 because overflow)
	if(lives == 255)
		draw_string_P(PSTR(STR_GAMEOVER), false, 34, 32);  //������Ϸ����

	return DISPLAY_BUSY;  //������Ļˢ��æ
}

#endif
