#ifndef _MPU_TASK_H_
#define _MPU_TASK_H_

#include "sys.h"

extern float pitch;
extern float roll;
extern float yaw;
	
void mpu_open(void);
void mpu_updata(void);
void myOLED_DrawLine(s16 x0, s16 y0, s16 x1, s16 y1,u8 t);

#endif
