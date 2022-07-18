#ifndef _TXT_VIEWER_H_
#define _TXT_VIEWER_H_

#include "sys.h"
#include "stdbool.h"
#include "string.h"

typedef struct disp_controlBlock{
	//��ʼ��ʾ�ַ�������ֵ��Ҳ������Ļ�е�һ����ʾ���ַ��ڻ����е�λ��
	u16 start_charIdx;
	//�����ַ�������ֵ
	u16 store_charIdx;
	
}disp_cb_t;

struct txtViewer{
	s16 txt_ofsetX;
	s16 txt_ofsetY;
	s16 window_x;
	s16 window_y;
	s16 window_h;
	s16 window_w;
	u8 char_h;
	u8 char_w;
	char *data_buf;
	u16 buf_size;
	disp_cb_t disp_cb;
};

typedef struct txtViewer txtViewer_t;

void txt_test_open(void);

#endif
