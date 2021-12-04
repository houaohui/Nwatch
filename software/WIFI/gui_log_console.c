#include "gui_log_console.h"
#include "usart2.h"
#include "delay.h"
#include "common.h"
#include "lcd.h"


#define LINE_BUF_MAX  20
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

unsigned char show_last_line_ldx=0;  //偏移行数，控制该变量可以看历史行,


void gui_print_s(int x, int y,char *str)
{
	draw_string(str, false, x, y);
}


bool up_line(void)
{
	show_last_line_ldx++;
	if(show_last_line_ldx>=LINE_BUF_MAX-LINE_SHOW_CNT)  //最大偏移行是20-8，再偏移第一行就显示到最后一行了
	{
		show_last_line_ldx = LINE_BUF_MAX-LINE_SHOW_CNT;
	}
	return true;
}

bool down_line(void)
{
	show_last_line_ldx--;
	if(show_last_line_ldx==255)
	{
		show_last_line_ldx=0;
	}
	return true;
}


void console_loop_show(void)
{
	char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //得到没有偏移的第一行索引值
	
	show_index=(LINE_BUF_MAX+show_index-show_last_line_ldx)%LINE_BUF_MAX;  //在此索引值上偏移show_last_line_ldx
	
	for(char c=1;c<=LINE_SHOW_CNT;c++)  //显示所有行
	{
		gui_print_s(0,LINE(c),string_buff[show_index].str_buff);  //依次从第一行显示到第八行
		show_index++;
		if(show_index>=LINE_BUF_MAX)show_index=0;
	}
}

void strcopy_cnt(char* buf, char* src ,char cnt)
{
	char i=0;
	for(i=0;i<cnt;i++)
	{
		buf[i]=src[i];
	}
	buf[i]='\0';
}

short find_huanhang_str_idx(char* src,unsigned short cnt)
{
	for(short i=0;i<=cnt;i++)
	{
		if(src[i]=='\0')
		{
			break;
		}
		if(src[i]=='\n')
		{
			return i;//指向当前字符
		}
	}
	return -1;
}

#define SUPORT_HUANGHANG 1

void console_log(unsigned short time_delay ,char* fmt,...)
{
	
	char div_flag=0;  //分行显示标志位
	va_list ap;
	va_start(ap,fmt);
	vsprintf(firststr_buff,fmt,ap);
	va_end(ap);	
	
	
	show_last_line_ldx=0;  //清除偏移
	
	unsigned short All_str_length=strlen((const char*)firststr_buff);	//计算显示总字符长度
	
#if SUPORT_HUANGHANG  //支持换行
	
	//换行字符de位置,搜索LINE_CHAR_CNT个字符，要减去1
	short this_line_char_cnt=find_huanhang_str_idx(firststr_buff,LINE_CHAR_CNT-1);
	

	//如果找到，则新行显示
	if(this_line_char_cnt!=-1) 
	{
		//LINE_CHAR_CNT字符内有\n,并且\n不在结尾
		if(this_line_char_cnt+1!=All_str_length)
		{
			div_flag=1;
			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt); 
			this_line_char_cnt++; //换行在下行不显示
		}
		else  //LINE_CHAR_CNT字符内有\n,\n在结尾或者在第一个
		{
			div_flag=0;
			if(this_line_char_cnt==0)
				strcopy_cnt(string_buff[index].str_buff," ",1);
			else
				strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt); 
		}
	}
	else //没有找到，则最大行显示
	{
		div_flag=All_str_length/LINE_CHAR_CNT;
		
		//使得显示LINE_CHAR_CNT个字符时也不换行
		if(All_str_length==LINE_CHAR_CNT)
			div_flag=0;
		this_line_char_cnt = LINE_CHAR_CNT;
		strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt); 
	}
	
	
	
	
	
	
#else

	div_flag=All_str_length/LINE_CHAR_CNT;  //显示新行标志
	
	//储存当前字符串
	strcopy_cnt(string_buff[index].str_buff,firststr_buff,LINE_CHAR_CNT);
#endif
	
	now_str_length = strlen((const char*)string_buff[index].str_buff);
	
	if(now_str_length<last_str_length)
	{
		for(char i=0;i<last_str_length-now_str_length;i++)
		{
			strcat(string_buff[index].str_buff," ");  //加入空格清空上一行的显存
		}
	}
	last_str_length=now_str_length;
	

	index++;
	
	if(index>=LINE_BUF_MAX)  //循环储存字符
	{
		index=0;
	}
	
	if(index>=LINE_SHOW_CNT)  //大于要显示的行数，就要显示新行
	{
		new_line_flag=1;
	}
	
	if(new_line_flag==0) //直接在下一行打印新行
	{
		gui_print_s(0,LINE(index),string_buff[index-1].str_buff);
	}
	else
	{
		memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);
		
		
		char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //定位最后一行
		for(char c=1;c<=LINE_SHOW_CNT;c++)  //显示所有行
		{
			gui_print_s(0,LINE(c),string_buff[show_index].str_buff);  //依次从第一行显示到第八行
			show_index++;
			if(show_index>=LINE_BUF_MAX)show_index=0;
		}
		
	}
	
	oled_flush();	
	
	delay_ms(time_delay);
	
#if SUPORT_HUANGHANG  //支持换行
	
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

/////////还没有添加新行功能
//void console_log_strbuff(unsigned short time_delay ,char* str)
//{

//	sprintf(string_buff[index].str_buff,str);
//	
//	
//	now_str_length=strlen((const char*)string_buff[index].str_buff);	
//	if(now_str_length<last_str_length)
//	{
//		for(char i=0;i<last_str_length-now_str_length;i++)
//		{
//			strcat(string_buff[index].str_buff," ");
//		}
//	}
//	last_str_length=now_str_length;
//	
//	index++;
//	if(index>=LINE_MAX)
//	{
//		index=0;
//		new_line_flag=1;
//	}
//	
//	if(new_line_flag==0)
//	{
//		gui_print_s(0,LINE(index),string_buff[index-1].str_buff);
//	}
//	else
//	{
//		char show_index=index;
//		for(char c=1;c<=LINE_MAX;c++)  //显示所有行
//		{
//			gui_print_s(0,LINE(c),string_buff[show_index].str_buff);  //依次从第一行显示到第八行
//			show_index++;
//			if(show_index>=LINE_MAX)show_index=0;
//		}
//		
//	}
//	
//	oled_flush();	
//	
//	delay_ms(time_delay);
//	
//}







