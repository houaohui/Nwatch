/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"
#include "led.h"
#include "lcd.h"

inline static void setBuffByte(byte*, byte, byte, byte);//, byte);
inline uint8_t pgm_read_byte (const uint8_t *abc) {return *abc;}

void draw_string_P(const char* string, bool invert, byte x, byte y)
{
	byte len = strlen(string);
	char buff[len+1];
	strcpy(buff, string);
	draw_string(buff, invert, x, y);
}

void draw_string(char* string, bool invert, byte x, byte y)
{
	byte charCount = 0;
	while(*string)
	{
		char c = *string - 0x20;
		byte xx = x + (charCount*7);
		draw_bitmap(xx, y, smallFont[(byte)c], SMALLFONT_WIDTH, SMALLFONT_HEIGHT, invert, 0);
		if(invert)
		{
			if(xx > 0)
				setBuffByte(oledBuffer, xx-1, y, 0xFF);//, WHITE);
			if(xx < FRAME_WIDTH - 5)
				setBuffByte(oledBuffer, xx+5, y, 0xFF);//, WHITE);
		}
		string++;
		charCount++;
	}
}

inline static void setBuffByte(byte* buff, byte x, byte y, byte val)//, byte colour)
{
	uint pos = x + (y / 8) * FRAME_WIDTH;
	buff[pos] |= val;
}

inline static byte readPixels(const byte* loc, bool invert)
{
	//byte pixels = ((int)loc & 0x8000 ? eeprom_read_byte((int)loc & ~0x8000) : pgm_read_byte(loc));
	byte pixels = pgm_read_byte(loc);  //d��ȡflash��������ݵ�ram
	if(invert)
		pixels = ~pixels;
	return pixels;
}

// Ultra fast bitmap drawing
// Only downside is that height must be a multiple of 8, otherwise it gets rounded down to the nearest multiple of 8
// Drawing bitmaps that are completely on-screen and have a Y co-ordinate that is a multiple of 8 results in best performance
// PS - Sorry about the poorly named variables ;_;
//����λͼ����
//Ψһ��ȱ���Ǹ߶ȱ�����8�ı����������������뵽��ӽ�8�ı���
//��λͼ����ȫ����Ļ�ϣ���һ��Y������8�ı������������
// PS -������˼��������������;
void draw_bitmap(byte x, byte yy, const byte* bitmap, byte w, byte h, bool invert, byte offsetY)
{
	// Apply animation offset
	yy += animation_offsetY();

	// 
	byte y = yy - offsetY;

	// 
	byte h2 = h / 8;
	
	// 
	byte pixelOffset = (y % 8);

	byte thing3 = (yy+h);
	
	// 
	LOOP(h2, hh)
	{
		// 
		byte hhh = (hh * 8) + y; // Current Y pos (every 8 pixels)
		byte hhhh = hhh + 8; // Y pos at end of pixel column (8 pixels)

		// 
		if(offsetY && (hhhh < yy || hhhh > FRAME_HEIGHT || hhh > thing3))
			continue;

		// 
		byte offsetMask = 0xFF;
		if(offsetY)
		{
			if(hhh < yy)
				offsetMask = (0xFF<<(yy-hhh));
			else if(hhhh > thing3)
				offsetMask = (0xFF>>(hhhh-thing3));
		}

		uint aa = ((hhh / 8) * FRAME_WIDTH);
		
		const byte* b = bitmap + (hh*w);
		
		// If() outside of loop makes it faster (doesn't have to keep re-evaluating it)
		// Downside is code duplication
		if(!pixelOffset && hhh < FRAME_HEIGHT)
		{
			// 
			LOOP(w, ww)
			{
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
			
				// Stop if X co-ordinate is outside the frame
				if(xx >= FRAME_WIDTH)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask;

				oledBuffer[xx + aa] |= pixels;

				//setBuffByte(buff, xx, hhh, pixels, colour);
			}
		}
		else
		{
			uint aaa = ((hhhh / 8) * FRAME_WIDTH);
			
			// 
			LOOP(w, ww)
			{
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
		
				// Stop if X co-ordinate is outside the frame
				if(xx >= FRAME_WIDTH)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask;

				// 
				if(hhh < FRAME_HEIGHT)
					oledBuffer[xx + aa] |= pixels << pixelOffset;
					//setBuffByte(buff, xx, hhh, pixels << pixelOffset, colour);				

				// 
				if(hhhh < FRAME_HEIGHT)
					oledBuffer[xx + aaa] |= pixels >> (8 - pixelOffset);
					//setBuffByte(buff, xx, hhhh, pixels >> (8 - pixelOffset), colour);		
			}
		}
	}
}

u8 moveLowBits(u8 n)
{
	u8 i=0xff;
	while(n-- && i!=0)
	{
		i = i << 1;
	}
	return i;
}

u8 moveLHigBits(u8 n)
{
	u8 i=0xff;
	while(n-- && i!=0)
	{
		i = i >> 1;
	}
	return i;
}

// ��ʵ�ϣ����Ը��ݻ�ͼ����д��һ�����������ͼ����������ʾ����������̫������
// �������ƻ�ͼ���������ڶԽ�����Ϊ: (start_x, start_y),(FRAME_HEIGHT_H,FRAME_WIDTH_W),����������...
void mydraw_bitmap(s16 x, s16 yy, const byte* bitmap, byte w, byte h, bool invert, byte offsetY,s16 start_x,s16 start_y, s16 FRAME_HEIGHT_H,s16 FRAME_WIDTH_W)
{
	// Apply animation offset
	yy += animation_offsetY();

	// ���� y������������ģ���ΪҪ��������λͼ������
	byte y = yy ;

	// 
	byte h2 = h / 8;
	
	// 
	byte pixelOffset = (y % 8);
	// ���㴰���и���Ҫ������ͼ������λ��
	s16 LowBitsNeedToClear = 0;
	s16 HigBitsNeedToClear = 0;
	// Ӧ����֧�����д�С��ͼ�Σ�û�в���
	if(yy < start_y && start_y < yy + h)
	{
		LowBitsNeedToClear = start_y - yy;
	}
	
	if(y < FRAME_HEIGHT_H && FRAME_HEIGHT_H < y + h)
	{
		HigBitsNeedToClear = y + h - FRAME_HEIGHT_H;
	}

	byte thing3 = (yy+h);
	
	// ÿһ��
	LOOP(h2, hh)
	{
		// 
		byte hhh = (hh * 8) + y; // Current Y pos (every 8 pixels)
		byte hhhh = hhh + 8; // Y pos at end of pixel column (8 pixels)

		// 
		if(offsetY && (hhhh < yy || hhhh > FRAME_HEIGHT || hhh > thing3))
			continue;

		// 
		byte offsetMask = 0xFF;
		if(offsetY)
		{
			if(hhh < yy)
				offsetMask = (0xFF<<(yy-hhh));
			else if(hhhh > thing3)
				offsetMask = (0xFF>>(hhhh-thing3));
		}
		//ƫ��128���ֽ�
		uint aa = ((hhh / 8) * FRAME_WIDTH);
		//ͼ���ƫ��
		const byte* b = bitmap + (hh*w);
		
		// If() outside of loop makes it faster (doesn't have to keep re-evaluating it)
		// Downside is code duplication
		// y�����ܱ�8����
		
		
		if(!pixelOffset )
		{
			// 
			LOOP(w, ww)
			{
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
			
				// Stop if X co-ordinate is outside the frame
				if(xx >= FRAME_WIDTH_W || xx < start_x || xx >= FRAME_WIDTH)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask & moveLowBits(LowBitsNeedToClear) & moveLHigBits(HigBitsNeedToClear);

				if(hhh < FRAME_HEIGHT_H )
				oledBuffer[xx + aa] |= pixels;

				//setBuffByte(buff, xx, hhh, pixels, colour);
			}
		}
		else
		{
			uint aaa = ((hhhh / 8) * FRAME_WIDTH);
			
			// 
			LOOP(w, ww)
			{
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
		
				// Stop if X co-ordinate is outside the frame
				if(xx >= FRAME_WIDTH_W || xx < start_x || xx >= FRAME_WIDTH)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask & moveLowBits(LowBitsNeedToClear) & moveLHigBits(HigBitsNeedToClear);

				// �ڴ��ڷ�Χ�ڱ���Ҫ��ȫ
				if(hhh < FRAME_HEIGHT_H ) {
					oledBuffer[xx + aa] |= pixels << pixelOffset;
					//setBuffByte(buff, xx, hhh, pixels << pixelOffset, colour);				

				// 
				// if(hhhh < FRAME_HEIGHT_H )
					oledBuffer[xx + aaa] |= pixels >> (8 - pixelOffset);
				}
				if(hhhh < FRAME_HEIGHT_H ) //�°벿�ֻ���Ҫ��
					oledBuffer[xx + aaa] |= pixels >> (8 - pixelOffset);
				
			}
		}
		if(LowBitsNeedToClear >= 8)
			LowBitsNeedToClear = LowBitsNeedToClear - 8;
		else
			LowBitsNeedToClear = 0;
		if(HigBitsNeedToClear >= 8)
			HigBitsNeedToClear = HigBitsNeedToClear - 8;
		else
			HigBitsNeedToClear = 0;
	}
}

// y must be a multiple of 8
// height is always 8
void draw_clearArea(byte x, byte y, byte w)
{
	uint pos = x + (y / 8) * FRAME_WIDTH;
	memset(&oledBuffer[pos], 0x00, w);


}

void draw_end()
{
	 oled_flush();  //ˢ����Ļ����˼

}
