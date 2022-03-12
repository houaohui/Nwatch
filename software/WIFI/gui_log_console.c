#include "gui_log_console.h"
#include "usart2.h"
#include "delay.h"
#include "common.h"
#include "lcd.h"


#define LINE_BUF_MAX  50
#define LINE_SHOW_CNT 8 
#define LINE_CHAR_CNT 17

#define LINE(n)              ((n-1)*8)

STRBUFF string_buff[LINE_BUF_MAX];
char firststr_buff[1024];

char now_line=1;
char new_line_flag=0;

static char index=0;
static unsigned char last_str_length=0;
char now_str_length;

unsigned char show_last_line_ldx=0;  //ƫ�����������Ƹñ������Կ���ʷ��,


void gui_print_s(int x, int y,char *str)
{
	draw_string(str, false, x, y);
}

//��ǰ�İ����ص�����
//bool up_line(void)
//{
//	show_last_line_ldx++;
//	if(show_last_line_ldx>=LINE_BUF_MAX-LINE_SHOW_CNT)  //���ƫ������20-8����ƫ�Ƶ�һ�о���ʾ�����һ����
//	{
//		show_last_line_ldx = LINE_BUF_MAX-LINE_SHOW_CNT;
//	}
//	return true;
//}
//��ǰ�İ����ص�����
//bool down_line(void)
//{
//	show_last_line_ldx--;
//	if(show_last_line_ldx==255)
//	{
//		show_last_line_ldx=0;
//	}
//	return true;
//}

//��ʾ���ؼ�ƫ��64-160
short pix_show_idx=64;

#define CHAR_HEIGHT 8


//���ؼ��İ����ص�����
bool up_line(void)
{
	pix_show_idx++;
	if(pix_show_idx>=CHAR_HEIGHT*LINE_BUF_MAX) 
	{
		pix_show_idx =CHAR_HEIGHT*LINE_BUF_MAX;
	}
	return false;
}
//���ؼ��İ����ص�����
bool down_line(void)
{
	pix_show_idx--;
	if(pix_show_idx<=CHAR_HEIGHT*LINE_SHOW_CNT)
	{
		pix_show_idx=CHAR_HEIGHT*LINE_SHOW_CNT;
	}
	return false;
}


//�˺����滻����ǰ�Ĺ��������������ؼ������������Ҫ��ǰ�ķ�ʽ��ע����������������ǰ��
void console_loop_show(void)
{
	
	char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //��һ������ֵ
	
	//�õ������show_index���ϵ�ƫ������
	short pix_offset_line=(pix_show_idx-CHAR_HEIGHT*LINE_SHOW_CNT)/CHAR_HEIGHT+1;   
	
	show_index=(LINE_BUF_MAX+show_index-pix_offset_line)%LINE_BUF_MAX;
	
	
	for(char c=0;c<=LINE_SHOW_CNT;c++)  
	{
		gui_print_s(0,LINE(c)+(pix_show_idx-CHAR_HEIGHT*LINE_SHOW_CNT)%CHAR_HEIGHT,string_buff[show_index].str_buff);  //���δӵ�һ����ʾ���ڰ���
		show_index++;
		if(show_index>=LINE_BUF_MAX)show_index=0;
	}
}


//void console_loop_show(void)
//{
//	char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //�õ�û��ƫ�Ƶĵ�һ������ֵ
//	
//	show_index=(LINE_BUF_MAX+show_index-show_last_line_ldx)%LINE_BUF_MAX;  //�ڴ�����ֵ��ƫ��show_last_line_ldx
//	
//	for(char c=1;c<=LINE_SHOW_CNT;c++)  //��ʾ������
//	{
//		gui_print_s(0,LINE(c),string_buff[show_index].str_buff);  //���δӵ�һ����ʾ���ڰ���
//		show_index++;
//		if(show_index>=LINE_BUF_MAX)show_index=0;
//	}
//}
//copycnt-1���ַ�
void strcopy_cnt(char* buf, char* src ,char cnt)
{
	char i=0;
	for(i=0;i<cnt;i++)
	{
		buf[i]=src[i];
	}
	buf[i]='\0';
}


//���\nǰ���\r
char r_off_set=0;
char r_kee=0;

short find_huanhang_str_idx(char* src,unsigned short cnt)
{
	r_off_set=0;	
	for(short i=0;i<cnt;i++)
	{
		if(src[i]=='\0')
		{
			break;
		}
		if(src[i]=='\n')
		{
			
			if(src[i-1]=='\r')
			{
				r_off_set=1;
			}

			return i;//ָ��ǰ�ַ�
		}
		//���\r�պ������һ���ַ�����ʱ��������\n
		//ֻ���������ģ����е�����\r��ֱ����ʾ����
		if(src[cnt-1]=='\r'&&src[cnt]=='\n')
		{
			r_kee=1;
			r_off_set=1;
		}
	}
	return -1;
}

#define SUPORT_HUANGHANG 1

void console_log(unsigned short time_delay ,char* fmt,...)
{
	
	char div_flag=0;  //������ʾ��־λ
	va_list ap;
	va_start(ap,fmt);
	vsprintf(firststr_buff,fmt,ap);
	va_end(ap);	
	
	
	show_last_line_ldx=0;  //���ƫ��
	pix_show_idx=CHAR_HEIGHT*LINE_SHOW_CNT;
	
	unsigned short All_str_length=strlen((const char*)firststr_buff);	//������ʾ���ַ�����
	
	if(All_str_length==0)
	{
		return;
	}
	
#if SUPORT_HUANGHANG  //֧�ֻ���
	
	//�����ַ�deλ��,����LINE_CHAR_CNT���ַ�
	short this_line_char_cnt=find_huanhang_str_idx(firststr_buff,LINE_CHAR_CNT);
	
	//LINE_CHAR_CNT���ַ�����\n
	if(this_line_char_cnt!=-1) 
	{

		{
			div_flag=1;
			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
			this_line_char_cnt++; //���������в���ʾ
		}

	}
	//LINE_CHAR_CNT���ַ���û��\n
	else
	{
		div_flag=All_str_length/LINE_CHAR_CNT;
		
		if(r_kee)
		{
			r_kee=0;
			//����\r�����\n
			this_line_char_cnt = LINE_CHAR_CNT;
			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
			this_line_char_cnt = LINE_CHAR_CNT+1;
		}
		else
		{
			this_line_char_cnt = LINE_CHAR_CNT;
			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
		}
	}
	
	
//	//����ҵ�����������ʾ
//	if(this_line_char_cnt!=-1) 
//	{
//		//LINE_CHAR_CNT�ַ���de\n���ڽ�β
//		if(this_line_char_cnt+1!=All_str_length)
//		{
//			div_flag=1;
//			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
//			this_line_char_cnt++; //���������в���ʾ
//		}
//		else  //LINE_CHAR_CNT�ַ���de\n�ڽ�β�����ڵ�һ��
//		{
//			div_flag=0;
//			if(this_line_char_cnt==0)
//				strcopy_cnt(string_buff[index].str_buff," ",1);
//			else
//				strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
//		}
//	}
//	else //û���ҵ������������ʾ
//	{
//		div_flag=All_str_length/LINE_CHAR_CNT;
//		
//		//ʹ����ʾLINE_CHAR_CNT���ַ�ʱҲ������
//		if(All_str_length==LINE_CHAR_CNT)
//			div_flag=0;
//		this_line_char_cnt = LINE_CHAR_CNT;
//		strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt); 
//	}
//	
	
	
	
	
	
#else

	div_flag=All_str_length/LINE_CHAR_CNT;  //��ʾ���б�־
	
	//���浱ǰ�ַ���
	strcopy_cnt(string_buff[index].str_buff,firststr_buff,LINE_CHAR_CNT);
#endif
	
	now_str_length = strlen((const char*)string_buff[index].str_buff);
	
	if(now_str_length<last_str_length)
	{
		for(char i=0;i<last_str_length-now_str_length;i++)
		{
			strcat(string_buff[index].str_buff," ");  //����ո������һ�е��Դ�
		}
	}
	last_str_length=now_str_length;
	

	index++;
	
	if(index>=LINE_BUF_MAX)  //ѭ�������ַ�
	{
		index=0;
	}
	
	if(index>=LINE_SHOW_CNT)  //����Ҫ��ʾ����������Ҫ��ʾ����
	{
		new_line_flag=1;
	}
	
	if(new_line_flag==0) //ֱ������һ�д�ӡ����
	{
		gui_print_s(0,LINE(index),string_buff[index-1].str_buff);
	}
	else
	{
		memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);
		
		
		char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //��λ���һ��
		for(char c=1;c<=LINE_SHOW_CNT;c++)  //��ʾ������
		{
			gui_print_s(0,LINE(c),string_buff[show_index].str_buff);  //���δӵ�һ����ʾ���ڰ���
			show_index++;
			if(show_index>=LINE_BUF_MAX)show_index=0;
		}
		
	}
	
	oled_flush();	
	
	delay_ms(time_delay);
	
#if SUPORT_HUANGHANG  //֧�ֻ���
	
	if(div_flag)
	{
		console_log(300,"%s",&firststr_buff[this_line_char_cnt]);
	}
	
#else
	
	
	if(div_flag)
	{
		console_log(300,"%s",&firststr_buff[LINE_CHAR_CNT]);
	}
	
#endif
	
}






