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

unsigned char show_last_line_ldx=0;  //偏移行数，控制该变量可以看历史行,


void gui_print_s(int x, int y,char *str)
{
	draw_string(str, false, x, y);
}

//以前的按键回调函数
//bool up_line(void)
//{
//	show_last_line_ldx++;
//	if(show_last_line_ldx>=LINE_BUF_MAX-LINE_SHOW_CNT)  //最大偏移行是20-8，再偏移第一行就显示到最后一行了
//	{
//		show_last_line_ldx = LINE_BUF_MAX-LINE_SHOW_CNT;
//	}
//	return true;
//}
//以前的按键回调函数
//bool down_line(void)
//{
//	show_last_line_ldx--;
//	if(show_last_line_ldx==255)
//	{
//		show_last_line_ldx=0;
//	}
//	return true;
//}

//显示像素级偏移64-160
short pix_show_idx=64;

#define CHAR_HEIGHT 8


//像素级的按键回调函数
bool up_line(void)
{
	pix_show_idx++;
	if(pix_show_idx>=CHAR_HEIGHT*LINE_BUF_MAX) 
	{
		pix_show_idx =CHAR_HEIGHT*LINE_BUF_MAX;
	}
	return false;
}
//像素级的按键回调函数
bool down_line(void)
{
	pix_show_idx--;
	if(pix_show_idx<=CHAR_HEIGHT*LINE_SHOW_CNT)
	{
		pix_show_idx=CHAR_HEIGHT*LINE_SHOW_CNT;
	}
	return false;
}


//此函数替换了以前的滚动，现在是像素级滚动，如果想要以前的方式，注释这个，用下面的以前的
void console_loop_show(void)
{
	
	char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //第一行索引值
	
	//得到相对于show_index向上的偏移行数
	short pix_offset_line=(pix_show_idx-CHAR_HEIGHT*LINE_SHOW_CNT)/CHAR_HEIGHT+1;   
	
	show_index=(LINE_BUF_MAX+show_index-pix_offset_line)%LINE_BUF_MAX;
	
	
	for(char c=0;c<=LINE_SHOW_CNT;c++)  
	{
		gui_print_s(0,LINE(c)+(pix_show_idx-CHAR_HEIGHT*LINE_SHOW_CNT)%CHAR_HEIGHT,string_buff[show_index].str_buff);  //依次从第一行显示到第八行
		show_index++;
		if(show_index>=LINE_BUF_MAX)show_index=0;
	}
}


//void console_loop_show(void)
//{
//	char show_index=(LINE_BUF_MAX+index-LINE_SHOW_CNT)%LINE_BUF_MAX;  //得到没有偏移的第一行索引值
//	
//	show_index=(LINE_BUF_MAX+show_index-show_last_line_ldx)%LINE_BUF_MAX;  //在此索引值上偏移show_last_line_ldx
//	
//	for(char c=1;c<=LINE_SHOW_CNT;c++)  //显示所有行
//	{
//		gui_print_s(0,LINE(c),string_buff[show_index].str_buff);  //依次从第一行显示到第八行
//		show_index++;
//		if(show_index>=LINE_BUF_MAX)show_index=0;
//	}
//}
//copycnt-1个字符
void strcopy_cnt(char* buf, char* src ,char cnt)
{
	char i=0;
	for(i=0;i<cnt;i++)
	{
		buf[i]=src[i];
	}
	buf[i]='\0';
}


//解决\n前面的\r
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

			return i;//指向当前字符
		}
		//解决\r刚好在最后一个字符，此时搜索不到\n
		//只处理连续的，若有单独的\r就直接显示出来
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
	
	char div_flag=0;  //分行显示标志位
	va_list ap;
	va_start(ap,fmt);
	vsprintf(firststr_buff,fmt,ap);
	va_end(ap);	
	
	
	show_last_line_ldx=0;  //清除偏移
	pix_show_idx=CHAR_HEIGHT*LINE_SHOW_CNT;
	
	unsigned short All_str_length=strlen((const char*)firststr_buff);	//计算显示总字符长度
	
	if(All_str_length==0)
	{
		return;
	}
	
#if SUPORT_HUANGHANG  //支持换行
	
	//换行字符de位置,搜索LINE_CHAR_CNT个字符
	short this_line_char_cnt=find_huanhang_str_idx(firststr_buff,LINE_CHAR_CNT);
	
	//LINE_CHAR_CNT个字符中有\n
	if(this_line_char_cnt!=-1) 
	{

		{
			div_flag=1;
			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
			this_line_char_cnt++; //换行在下行不显示
		}

	}
	//LINE_CHAR_CNT个字符中没有\n
	else
	{
		div_flag=All_str_length/LINE_CHAR_CNT;
		
		if(r_kee)
		{
			r_kee=0;
			//跳过\r后面的\n
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
	
	
//	//如果找到，则新行显示
//	if(this_line_char_cnt!=-1) 
//	{
//		//LINE_CHAR_CNT字符内de\n不在结尾
//		if(this_line_char_cnt+1!=All_str_length)
//		{
//			div_flag=1;
//			strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
//			this_line_char_cnt++; //换行在下行不显示
//		}
//		else  //LINE_CHAR_CNT字符内de\n在结尾或者在第一个
//		{
//			div_flag=0;
//			if(this_line_char_cnt==0)
//				strcopy_cnt(string_buff[index].str_buff," ",1);
//			else
//				strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt-r_off_set); 
//		}
//	}
//	else //没有找到，则最大行显示
//	{
//		div_flag=All_str_length/LINE_CHAR_CNT;
//		
//		//使得显示LINE_CHAR_CNT个字符时也不换行
//		if(All_str_length==LINE_CHAR_CNT)
//			div_flag=0;
//		this_line_char_cnt = LINE_CHAR_CNT;
//		strcopy_cnt(string_buff[index].str_buff,firststr_buff,this_line_char_cnt); 
//	}
//	
	
	
	
	
	
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






