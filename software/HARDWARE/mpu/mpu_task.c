#include "mpu_task.h"
#include "common.h"

#ifndef _swap_char
#define _swap_char(a, b)     \
    {                        \
        u8 t = a; \
        a = b;               \
        b = t;               \
    }
#endif
	
void draw_point(u8 x,u8 y,u8 value)
{
	if(x>128-1 ||y>64-1)   //超出范围
		return;

		if(value)
			oledBuffer[(y/8)*128+x] |= 1<<(y%8);
		else
			oledBuffer[(y/8)*128+x] &= ~(1<<(y%8));	
}
//绘制两点线段至缓存
//t=1绘制线段绘制线段至缓存
//t=0清除线段绘制线段至缓存
void myOLED_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1,u8 t)
{
    signed char dx, dy, ystep;
    int err;
    u8 swapxy = 0;
	
	if (x0 > 127)
        return;
    if (y0 > 63)
        return;
    if (x1 > 127)
       return;
    if (y1 > 63)
        return;

//    if (x0 > 127)
//        x0 = 127;
//    if (y0 > 63)
//        y0 = 63;
//    if (x1 > 127)
//        x1 = 127;
//    if (y1 > 63)
//        y1 = 63;

    dx = abs(x1 - x0);//求绝对值
    dy = abs(y1 - y0);

    if (dy > dx)
    {
        swapxy = 1;
        _swap_char(dx, dy);
        _swap_char(x0, y0);
        _swap_char(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_char(x0, x1);
        _swap_char(y0, y1);
    }
    err = dx >> 1;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (swapxy == 0)
            draw_point(x0, y0,t);
        else
            draw_point(y0, x0,t);
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}



float pitch,roll,yaw; 	

display_t draw(void);

extern char imu_run;

void mpu_updata(void)
{
	static millis8_t lastUpdate;

	  // Update every 20ms
	  millis8_t now = millis();
	  if ((millis8_t)(now - lastUpdate) >= 50)
	  {
		 // printf("1");
		lastUpdate = now;
//		if(imu_run)
//		{
//			mpu_dmp_get_data(&pitch,&roll,&yaw);	
//			 //printf("pitch:%f,roll:%f,yaw:%f\r\n",pitch,roll,yaw);
//		}			
		 
	  }
	
}


extern bool keep_on;

char imu_run=0;

unsigned char delta=1;
unsigned char point_delta=1;


static bool btnExit()
{
	animation_start(display_load, ANIM_MOVE_OFF);
	keep_on=false;  //如果在draw里一直刷新变量，变量在这里关闭不了，因为会draw线程会再次覆盖
	imu_run=0;
	return true;
}


static bool btnup()
{
	point_delta--;
	if(point_delta==0)
	{
		point_delta=1;
		delta++;
	}
	return false;
}

static bool btndown()
{
	delta--;
	if(delta==0)
	{
		point_delta++;
		delta=1;
	}
	return false;
}

void mpu_open(void)
{
	display_setDrawFunc(draw);
	buttons_setFuncs(btnup, btnExit, btndown);
	beginAnimation2(NULL);
	keep_on = true;
	imu_run=1;
}




static void dso(unsigned char data,unsigned char delta)
{
	static unsigned char buf[1024];
	static unsigned short showindex=0,saveindex=0;
	saveindex=showindex+1;
	if(saveindex == 1024)
		saveindex=0;
	buf[saveindex]=data;
	//
	showindex++;
	if(showindex == 1024)
		showindex=0;
	//仅仅画点
	unsigned short cnt=0,showcnt;
	showcnt=showindex;
	for(cnt=0;  cnt <=127/delta ;cnt++)//屏幕宽度0-127
	{
		draw_point(127-(cnt*delta),buf[showcnt],1);
		showcnt=showcnt-point_delta;
		if(showcnt>=1024)
		{
			showcnt=1024-point_delta+showcnt;//回到数组尾部
		}
	}
	//把点连成线
	unsigned short line_showindex,next_line_showindex;
	line_showindex=showindex;
	for(cnt=0;  cnt <=127/delta ;cnt++)
	{
		
		next_line_showindex=line_showindex-point_delta;
		if(next_line_showindex>=1024)
		{
			next_line_showindex=1024-point_delta+next_line_showindex;
		}
		myOLED_DrawLine(127-(cnt*delta),buf[line_showindex],127-((cnt+1)*delta),buf[next_line_showindex],1);
		line_showindex=line_showindex-point_delta;
		if(line_showindex>=1024)
		{
			line_showindex=1024-point_delta+line_showindex;
		}
	}
	
}

#include "math.h"

static display_t draw()
{
	char buf[12];
	sprintf(buf,"pit:%.2f",pitch);
	draw_string((char*)buf, false, 0, 0);
	
	sprintf(buf,"rol:%.2f",roll);
	draw_string((char*)buf, false, 0, 8);
	
	sprintf(buf,"yaw:%.2f",yaw);
	draw_string((char*)buf, false, 0, 16);
	
	//static u8 i=0;
	//static float x=0;
	//x=x+0.05;
	//i=32*sin(x)+32;

	dso(roll,delta);
	return DISPLAY_BUSY;
}




