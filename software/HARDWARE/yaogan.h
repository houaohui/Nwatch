#ifndef _YAOGAN_H_
#define _YAOGAN_H_

#include "sys.h"

extern volatile u16 AD_N_Value[4];

void Yaogan_Init(void);

unsigned char Yaogan_Get_Left(void);
unsigned char Yaogan_Get_Right(void);


unsigned char Yaogan_scan(void);

#endif

