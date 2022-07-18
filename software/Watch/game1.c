/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#if COMPILE_GAME1

#define PLATFORM_WIDTH	12    //平台宽度
#define PLATFORM_HEIGHT	4     //平台高度
#define UPT_MOVE_NONE	0     //不移动
#define UPT_MOVE_RIGHT	1     //向右移动
#define UPT_MOVE_LEFT	2     //向左移动
#define BLOCK_COLS		32    //砖块列数
#define BLOCK_ROWS		5     //砖块行数
#define BLOCK_COUNT		(BLOCK_COLS * BLOCK_ROWS)  //砖块数量

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

static byte uptMove;    //移动状态
static s_ball ball;
static bool* blocks=NULL;
static byte lives;      //生命值
static uint score;      //得分
static byte platformX;  
static bool gameEnded;

extern bool keep_on;
//游戏开始
void game1_start()
{
	//menu_close();//里面有打开动画，要在添加一下
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
	if(blocks==NULL||gameEnded)  //为空或者游戏结束重新分配内存
	{
		if(gameEnded)
		free(blocks);
		blocks = calloc(BLOCK_COUNT, 1);  //void* calloc（unsigned int num，unsigned int size）内存的动态存储区中分配num个长度为size的连续空间
	}
									//函数返回一个指向分配起始地址的指针,calloc函数会自动将内存初始化为0
	lives = 3;
	score = 0;
	platformX = (FRAME_WIDTH / 2) - (PLATFORM_WIDTH / 2);    //平台X坐标（绘制平台开始坐标） = 屏幕宽度/2减去平台宽度/2
}

#define OPTION_COUNT 3
static prev_menu_s prevMenuData;
static void itemLoader(byte num);
static void mSelect(void);

static void game1_meun(void)
{
	//menuData.isOpen = true;  //打开菜单
	setPrevMenuOpen(&prevMenuData, game1_meun);//多次进入这个菜单，上级菜单也不会被覆盖，上级菜单应该是游戏选择
	buttons_setFuncs(menu_up, menu_select, menu_down);
	display_setDrawFunc(menu_draw);  //绑定绘制函数为menu_draw
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR("game setting"));   //获取当前菜单信息（选项个数，菜单类型是文字还是图标）
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //绑定菜单的函数,如前进后退选择确认    
	beginAnimation2(NULL);
}

//doAction执行的函数不能带开启动画，退出动画会被覆盖
static void mSelect(void)
{
	setPrevMenuExit(&prevMenuData);//存储上次菜单的选项
	doAction(true);  //带退出动画执行指令
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
	free(blocks);    //释放内存空间的函数
	keep_on = false;
	menuData.prevMenu = prevMenuData.last; // 强制指向上级菜单 
	back();
}

static void itemLoader(byte num)
{
	num = 0;
	setMenuOption_P(num++, PSTR("resume"), NULL, resume);
	setMenuOption_P(num++, PSTR("auto mode"), NULL, auto_mode);
	setMenuOption_P(num++, PSTR("exit game"), NULL, game_exit);//与退出选项等价，不过可以改名字和添加一些代码
	addBackOption();
}


//游戏退出
static bool btnExit()
{
	free(blocks);    //释放内存空间的函数
	if(lives == 255)  //生命值变量溢出，即变为零
		game1_start();
	else
		animation_start(game1_meun, ANIM_MOVE_OFF);
	return true;
}
//向左移动
static bool btnRight()
{
	uptMove = UPT_MOVE_RIGHT;
	return false;
}
//向右移动
static bool btnLeft()
{
	uptMove = UPT_MOVE_LEFT;
	return false;
}
//游戏绘图
display_t game1_draw()
{
	gameEnded = ((score >= BLOCK_COUNT) || (lives == 255));  //游戏结束标志，当得分大于砖块数或者生命值溢出

	byte platformXtmp = platformX;        //缓存变量平台X坐标

	// Move platform
	if(uptMove == UPT_MOVE_RIGHT)    //向右移动
		platformXtmp += 3;
	else if(uptMove == UPT_MOVE_LEFT)   //向左移动
		platformXtmp -= 3;
	uptMove = UPT_MOVE_NONE;     //默认不移动

	// Make sure platform stays on screen
	if(platformXtmp > 250)
		platformXtmp = 0;
	else if(platformXtmp > FRAME_WIDTH - PLATFORM_WIDTH)
		platformXtmp = FRAME_WIDTH - PLATFORM_WIDTH;
	
	if(automode)
	{
		platformXtmp = ball.x + ball.velX;
	}

	// Draw platform 绘制平台图案
	draw_bitmap(platformXtmp, FRAME_HEIGHT - 8, platform, 12, 8, NOINVERT, 0);

	platformX = platformXtmp;

	// Move ball
	if(!gameEnded)   //如果游戏没有结束，小球继续移动
	{
		ball.x += ball.velX;
		ball.y += ball.velY;
	}

	bool blockCollide = false;   //碰撞标志
	const float ballX = ball.x;
	const byte ballY = ball.y;

	// Block collision    //块碰撞
	byte idx = 0;
	LOOP(BLOCK_COLS, x)  //for(x=0;x<BLOCK_COLS;x++)
	{
		LOOP(BLOCK_ROWS, y)
		{                         //小球与砖块的位置关系如果满足
			if(!blocks[idx] && ballX >= x * 4 && ballX < (x * 4) + 4 && ballY >= (y * 4) + 8 && ballY < (y * 4) + 8 + 4)
			{
				buzzer_buzz(100, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);    //蜂鸣器
//				led_flash(LED_GREEN, 50, 255);    //灯闪烁
				blocks[idx] = true;   //砖块消失
				blockCollide = true;  //碰撞标志位置一
				score++;    //得分加一
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

	// Platform collision 小球与平台相撞
	bool platformCollision = false;         //小球的Y坐标大于屏幕高度减去平台高度&&小球X坐标在平台起始坐标和平台起始坐标加上平台宽度之间
	if(!gameEnded && ballY >= FRAME_HEIGHT - PLATFORM_HEIGHT && ballY < 240 && ballX >= platformX && ballX <= platformX + PLATFORM_WIDTH)
	{
		platformCollision = true;
		buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL);
		ball.y = FRAME_HEIGHT - PLATFORM_HEIGHT;
		if(ball.velY > 0)
			ball.velY = -ball.velY;
		ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0   X方向速度随机生成
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

	// Draw ball  绘制小球
	draw_bitmap(ball.x, ball.y, ballImg, 2, 8, NOINVERT, 0);

	// Draw blocks  绘制砖块
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

	// Draw score   打印分数
	char buff[6];
	sprintf_P(buff, PSTR("%u"), score);
	draw_string(buff, false, 0, 0);

	// Draw lives   打印生命值
	if(lives != 255)
	{
		LOOP(lives, i)
			draw_bitmap((FRAME_WIDTH - (3*8)) + (8*i), 1, livesImg, 7, 8, NOINVERT, 0);
	}	

	// Got all blocks   //打碎所有砖块
	if(score >= BLOCK_COUNT)
		draw_string_P(PSTR(STR_WIN), false, 50, 32);  //绘制游戏胜利

	// No lives left (255 because overflow)
	if(lives == 255)
		draw_string_P(PSTR(STR_GAMEOVER), false, 34, 32);  //绘制游戏结束

	return DISPLAY_BUSY;  //返回屏幕刷新忙
}

#endif
