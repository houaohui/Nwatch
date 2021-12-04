#ifndef _MPU_TASK_H_
#define _MPU_TASK_H_

#include "sys.h"

extern float pitch;
extern float roll;
extern float yaw;
	
void mpu_open(void);
void mpu_updata(void);

#endif
