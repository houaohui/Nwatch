#include "TXT_Viewer.h"
#include <stdlib.h>
#include "draw.h"
#include "resources.h"
#include "gui_log_console.h"
#include "common.h"

#define FRAME_WIDTH			128
#define FRAME_HEIGHT		64

#define CHAR_BUF_MAX  1024
#define LINE_CHAR_MAX 17
#define LINE_CNT_MAX  8


#define CHAR_HEIGHT  8
#define CHAR_WITH    7
#define LINE(n)              ((n)*CHAR_HEIGHT)
#define COL(n)               ((n)*CHAR_WITH)

char * clang = "int main(int argc, char **argv)\n{\n\n    printf(\"hello world!\");\n    return 0;\n}\n";

//static u16 line(txtViewer_t *viewer, u16 n)
//{
//	return ((n)*viewer->char_h);
//}

//static u16 col(txtViewer_t *viewer, u16 n)
//{
//	return ((n)*viewer->char_w);
//}

int sizeof_str(char *str)
{
	int i=0;
	while(str[i++]!='\0');
	return i-1;
}


inline static void setBuffByte(byte* buff, byte x, byte y, byte val)//, byte colour)
{
	uint pos = x + (y / 8) * FRAME_WIDTH;
	buff[pos] |= val;
}
static void txtViewer_drawOneChar(char* string, bool invert, byte x, byte y)
{

    char c = *string - 0x20;
    //byte xx = x + (charCount*7);
    byte xx = x;
    draw_bitmap(xx, y, smallFont[(byte)c], SMALLFONT_WIDTH, SMALLFONT_HEIGHT, invert, 0);
    if(invert)
    {
        if(xx > 0)
            setBuffByte(oledBuffer, xx-1, y, 0xFF);//, WHITE);
        if(xx < FRAME_WIDTH - 5)
            setBuffByte(oledBuffer, xx+5, y, 0xFF);//, WHITE);
    }
    
}

static void txtViewer_printOneChar(int x, int y,char *str)
{
	txtViewer_drawOneChar(str, false, x, y);
}



//为TXT_VIewer初始化，为其分配缓存
void txtViewer_init(txtViewer_t *viewer, int size, s16 win_x, s16 win_y)
{
	viewer->char_h =  CHAR_HEIGHT;
	viewer->char_w =  CHAR_WITH;
	viewer->buf_size = size;
	viewer->txt_ofsetX = 0;
	viewer->txt_ofsetY = 0;
	viewer->window_x = win_x;
	viewer->window_y = win_y;
	viewer->window_h = 64;
	viewer->window_w = 128;
	viewer->data_buf = malloc(size);
	console_log(1,"malloc");
	if(viewer->data_buf == NULL)
		console_log(1,"malloc err!");
	else
		console_log(1,"success");
	console_log(1,"open Viewer!");
	
	strcat(viewer->data_buf,"hello,this is a test.\ncan you see something here?\nIf you can see some words\nThis test may have succeeded!\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n");
	strcat(viewer->data_buf,clang);
	viewer->disp_cb.start_charIdx = 0;
	viewer->disp_cb.store_charIdx = sizeof_str(viewer->data_buf);
}

void txtViewer_changeSource(txtViewer_t *viewer, char *destination)
{
	if(viewer->data_buf == NULL||viewer->data_buf == destination)
		return;
	
	free(viewer->data_buf);
	viewer->data_buf = destination;
}


//获取循环缓冲区的这个字符的下一个字符位置
static int getNext(txtViewer_t *viewer, u16 last_char_idx)
{
    last_char_idx++;
    if(last_char_idx >= viewer->buf_size)
    {
        last_char_idx = 0;
    }
    return last_char_idx;
}


static u16 findNewStartIdx(txtViewer_t *viewer)
{
	viewer->disp_cb.start_charIdx = getNext(viewer,viewer->disp_cb.start_charIdx);
	
	while( viewer->data_buf[viewer->disp_cb.start_charIdx] != '\n'
		&& viewer->disp_cb.start_charIdx != viewer->disp_cb.store_charIdx )
	{
		viewer->disp_cb.start_charIdx = getNext(viewer,viewer->disp_cb.start_charIdx);
	}
	
	if(viewer->disp_cb.start_charIdx == viewer->disp_cb.store_charIdx)
		return viewer->disp_cb.store_charIdx;
	else
		return getNext(viewer,viewer->disp_cb.start_charIdx);
	
}

static void txtViewer_addOneChar(txtViewer_t *viewer, char data)
{
	viewer->data_buf[viewer->disp_cb.store_charIdx] = data;
	viewer->disp_cb.store_charIdx = getNext(viewer,viewer->disp_cb.store_charIdx);
	
	//循环缓冲区的存储索引开始覆盖显示索引，取第二行第一个字符位置作为新的disp_cb.start_charIdx
	if(viewer->disp_cb.store_charIdx == viewer->disp_cb.start_charIdx)
		viewer->disp_cb.start_charIdx = findNewStartIdx(viewer);
}
//////////////////////////////////////////////////////

static u16 getFirstLineIdx(txtViewer_t *viewer,s16 skepCnt)
{
	u16 char_idx;
	char_idx = viewer->disp_cb.start_charIdx;
	char_idx = getNext(viewer, char_idx);
	if(skepCnt <= 0)
		return 0;
	
	//循环cnt次,找到要显示的第一行的第一个位置
	while(skepCnt--)
	{
		while( viewer->data_buf[char_idx] != '\n')
		{
			char_idx = getNext(viewer, char_idx);
		}
		char_idx = getNext(viewer, char_idx);
	}
	return char_idx;
}

void txtViewer_display(txtViewer_t *viewer)
{
	//计算需要跳过的行数
	s16 skipLineNum = viewer->txt_ofsetY/viewer->char_h;
	//获取窗口的第一行的第一个字符
	u16 char_idx = getFirstLineIdx(viewer,skipLineNum);
	
	//计算窗口每列要显示的字符个数
	u16 line_cnt = viewer->window_h/viewer->char_h+1;
	//计算窗口每行要显示的字符个数
	u16 col_cnt = viewer->window_w/viewer->char_w+1;
	//行打印计数器//列打印计数器
	u16 row_cnt = 0, col_char_cnt = 0;

	
	
	s16 eachLineCharOffsetNum = viewer->txt_ofsetX/viewer->char_w;
	
	//限制窗口字符行数打印
	while(row_cnt < line_cnt) {
		//解析每一个字符搜索换行，否则符合条件打印出来

			if(viewer->data_buf[char_idx] == '\n'){
				row_cnt++;
				col_char_cnt = 0;//遇到新的行列打印计数器清零
			}
			else {
				
				s16 txt_x = (col_char_cnt)*viewer->char_w - viewer->txt_ofsetX;
				s16 txt_y = (row_cnt + (skipLineNum > 0? skipLineNum:0))*viewer->char_h - viewer->txt_ofsetY;
				//限制窗口字符打印de列数
				if(col_char_cnt >= eachLineCharOffsetNum && col_char_cnt < col_cnt + eachLineCharOffsetNum) {
					
					if(col_char_cnt*viewer->char_w - viewer->txt_ofsetX > -viewer->char_w
						&&col_char_cnt*viewer->char_w < viewer->txt_ofsetX + viewer->window_w) {
						txtViewer_printOneChar(txt_x + viewer->window_x, txt_y + viewer->window_y,&viewer->data_buf[char_idx]);
					}
					
				}
				col_char_cnt++;
			}
			
			char_idx = getNext(viewer, char_idx);
			//当显示到达最新的存储字符位置时停止显示
			if(char_idx ==  viewer->disp_cb.store_charIdx||viewer->data_buf[char_idx] == '\0')
				break;
	}
}


void txtViewer_drawBoundary(txtViewer_t *viewer)
{
	myOLED_DrawLine(viewer->window_x, viewer->window_y, viewer->window_x+viewer->window_w, viewer->window_y,true);
	myOLED_DrawLine(viewer->window_x+viewer->window_w, viewer->window_y, viewer->window_x+viewer->window_w, viewer->window_y+viewer->window_h,true);
	myOLED_DrawLine(viewer->window_x, viewer->window_y+viewer->window_h, viewer->window_x+viewer->window_w, viewer->window_y+viewer->window_h,true);
	
	
	myOLED_DrawLine(viewer->window_x, viewer->window_y, viewer->window_x, viewer->window_y+viewer->window_h,true);
}



txtViewer_t txtViewer;
static bool scroll_down()
{
	txtViewer.txt_ofsetY++;
	return false;
}


static bool scroll_up()
{
	txtViewer.txt_ofsetY--;
	return false;
}

static bool scroll_left()
{
	txtViewer.txt_ofsetX--;
	return false;
	
}

static bool scroll_right()
{
	txtViewer.txt_ofsetX++;
	return false;
}

static bool win_up()
{
	txtViewer.window_y--;
	return false;
}
static bool win_down()
{
	txtViewer.window_y++;
	return false;
}
static bool win_left()
{
	txtViewer.window_x--;
	return false;
}
static bool win_right()
{
	txtViewer.window_x++;
	return false;
}

////////////////task test/////////////////////
extern bool keep_on;
static void txtViewer_meun(void);

static display_t draw(void);

static bool btnExit()
{
	//free(txtViewer.data_buf);
	animation_start(txtViewer_meun, ANIM_MOVE_OFF);
	return true;
}

button_f btndown = scroll_down;
button_f btnup = scroll_up;


extern unsigned char cmd_finish;
void txt_test_open(void)
{
	buttons_setFuncs(btndown, btnExit, btnup );
	beginAnimation2(NULL);
	if(txtViewer.data_buf == NULL)
	txtViewer_init(&txtViewer,256,0,0);
	display_setDrawFunc(draw);
	keep_on = true;
	cmd_finish=1;
}

static display_t draw()
{
	char buff[4];
	sprintf_P(buff, PSTR("%d"), txtViewer.txt_ofsetY);
	draw_string(buff, NOINVERT, 110, FRAME_HEIGHT - 8);
	sprintf_P(buff, PSTR("%d"), txtViewer.txt_ofsetX);
	draw_string(buff, NOINVERT, 110, FRAME_HEIGHT - 16);
	txtViewer_display(&txtViewer);
	txtViewer_drawBoundary(&txtViewer); 
	return DISPLAY_BUSY;
}

//////////////////////txt menu/////////////////////////////////
static prev_menu_s prevMenuData;
static void itemLoader(byte num);

static void mSelect(void);

static void txtViewer_meun(void)
{
	menuData.isOpen = true;  //打开菜单
	setPrevMenuOpen(&prevMenuData, txtViewer_meun);//多次进入这个菜单，上级菜单也不会被覆盖，上级菜单应该是游戏选择
	buttons_setFuncs(menu_up, menu_select, menu_down);
	display_setDrawFunc(menu_draw);  //绑定绘制函数为menu_draw
	setMenuInfo(8, MENU_TYPE_STR, PSTR("Viewer"));   //获取当前菜单信息（选项个数，菜单类型是文字还是图标）
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //绑定菜单的函数,如前进后退选择确认    
	beginAnimation2(NULL);
}

static void mSelect(void)
{
	setPrevMenuExit(&prevMenuData);//存储上次菜单的选项
	doAction(true);  //带退出动画执行指令
	menuData.isOpen = false;
}
void up_down()
{
	btndown = scroll_down;
	btnup = scroll_up;
	txt_test_open();
}
void left_right()
{
	btndown = scroll_left;
	btnup = scroll_right;txt_test_open();
}
void win_upDown()
{
	btndown = win_down;
	btnup = win_up;txt_test_open();
}
void win_leftRight()
{
	btndown = win_left;
	btnup = win_right;txt_test_open();
	
}
void size48x64()
{
	txtViewer.window_h = 48;
	txtViewer.window_w = 64;txt_test_open();
}

void size48x96()
{
	txtViewer.window_h = 48;
	txtViewer.window_w = 96;txt_test_open();
}

void size32x64()
{
	txtViewer.window_h = 32;
	txtViewer.window_w = 64;txt_test_open();
}
void sizefull()
{
	txtViewer.window_h = 64;
	txtViewer.window_w = 128;txt_test_open();
}

void backto()
{
	free(txtViewer.data_buf);
	wifi_test_open();
}
static void itemLoader(byte num)
{
	num = 0;
	setMenuOption_P(num++, PSTR("up down"), NULL, up_down);
	setMenuOption_P(num++, PSTR("left right"), NULL, left_right);
	setMenuOption_P(num++, PSTR("win up down"), NULL, win_upDown);
	setMenuOption_P(num++, PSTR("win left right"), NULL, win_leftRight);
	setMenuOption_P(num++, PSTR("win 48x64"), NULL, size48x64);
	setMenuOption_P(num++, PSTR("win 32x96"), NULL, size32x64);
	setMenuOption_P(num++, PSTR("win 48x96"), NULL, size48x96);
	setMenuOption_P(num++, PSTR("win full"), NULL, sizefull);
	setMenuOption_P(num++, PSTR("back"), NULL, backto);
}

