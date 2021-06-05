#include <stdio.h>
#include <stdarg.h>
#include "graph.h"

#include "font/ankfont.c"
#include "font/knjfont.c"
#include "font/knjtable.c"

#include "font.h"

static int flagSJIS = 0;

static unsigned short *Font_PixelBuffer;

void Font_Init(void)
{
	flagSJIS = 0;
	Font_PixelBuffer = (unsigned short *)graph_GetPixelArray();
}

void Font_UseSJIS(int flag)
{
	flagSJIS = flag;
}

int Font_IsSJIS1(unsigned char ch)
{
	if (0x81 <= ch && 0x9f >= ch)
		return 1;
	if (0xe0 <= ch)
		return 1;
		
	return 0;
}

int Font_IsSJIS2(unsigned char ch)
{
	if (0x40 <= ch && 0x7e >= ch)
		return 1;
	if (0x80 <= ch && 0xfc >= ch)
		return 1;

	return 0;
}

int Font_IsEUC(unsigned char ch)
{
	if (0xa1 <= ch && 0xfe >= ch)
		return 1;

	return 0;
}

unsigned short SJIS2JIS(unsigned short code)
{
	unsigned char b1,b2;
	
	b1 = (code>>8) & 0xff;
	b2 = code & 0xff;
	
	if (0x81 <= b1 && 0x9f >= b1)
		b1 -= 0x81;
	else
		b1 -= 0xc1;
	
	if (0x80 <= b2 && 0x9e >= b2)
		b2 -= 1;

	b1 <<=1;

	if (0x9f <= b2 && 0xfc >= b2)
	{
		b1 |= 1;
		b2 -= 0x7e;
	}
	else
		b2 -= 0x1f;
		
	b1+= 0x21;
	
	return ((unsigned short)b1 << 8) | b2;
}


void Font_PutKANJIEx(int color,int x,int y,int code,int bold)
{
	int i,j,pos;
	int bits;
	
	unsigned short rgb;
	
	bits = 0;
	rgb = color;

	code &= 0x7fff;
	pos = 32 * 
		((KANJITable[(code<<1)+1] << 8) |
		 (KANJITable[(code<<1)  ]));

	for(j=0; j < 16; j++)
	{
		for(i=0; i < 16; i++)
		{
			if (!(i & 0x7))
			{
				bits = KANJIFont[pos + (j*2)+(i/8)];
				if (bold)
					bits = bits | (bits<<1) | (bits>>1);
			}
			
			if ((x+i) >= 0 && (y+j) >= 0 && 
				(x+i) < SCR_WIDTH && (y+j) < SCR_HEIGHT && (bits & 0x80))
				Font_PixelBuffer[(BUF_WIDTH*(y+j)) + (x+i)] = rgb;
								
			bits<<=1;
		}
	}
}


void Font_PutCharEx(int color,int x,int y,int ch,int bold)
{
	int i,j,pos;
	int bits;
	
	unsigned short rgb;
	
	bits = 0;
	rgb = color;
			
	pos = 16 * (unsigned char)ch;
	for(j=0; j < 16; j++)
	{
		for(i=0; i < 8; i++)
		{
			if (!(i & 0xf))
			{
				bits = ANKFont[pos + j];
				if (bold)
					bits = bits | (bits<<1) | (bits>>1);
			}
			
			
			if ((x+i) >= 0 && (y+j) >= 0 && 
				(x+i) < SCR_WIDTH && (y+j) < SCR_HEIGHT && (bits & 0x80))
				Font_PixelBuffer[(BUF_WIDTH*(y+j)) + (x+i)] = rgb;

			bits<<=1;
		}
	}
}

void Font_PutKANJI(int color,int x,int y,int code)
{
	Font_PutKANJIEx(color,x,y,code,0);
}
void Font_PutChar(int color,int x,int y,int ch)
{
	Font_PutCharEx(color,x,y,ch,0);
}

void Font_PutStrSJIS(int color,int x,int y,const char *text)
{
	int i;
	int code;
	int bgc = MAKE_RGB15(0,0,0);

	for(i = 0; text[i]; i++)
	{
		if (text[i] == '\n')
		{
			y += 16;
			x = 0;
			continue;
		}
		
		if (Font_IsSJIS1(text[i]) && Font_IsSJIS2(text[i+1]))
		{
			code = ((unsigned char)text[i]<<8) | ((unsigned char)text[i+1]);
			code = SJIS2JIS(code);
			Font_PutKANJIEx(bgc  , x , y-1 , code ,1 );
			Font_PutKANJIEx(bgc  , x , y+1 , code ,1 );

			Font_PutKANJI(color, x , y , code);
			x += 16;
			i++;
		}
		else
		{
			Font_PutCharEx(bgc  , x , y-1 , text[i],1);
			Font_PutCharEx(bgc  , x , y+1 , text[i],1);
			Font_PutChar(color, x , y , text[i]);
			x += 8;
		}
	}
}


void Font_PutStrEUC(int color,int x,int y,const char *text)
{
	int i;
	int code;
	int bgc = MAKE_RGB15(0,0,0);
	
	for(i = 0; text[i]; i++)
	{
		if (text[i] == '\n')
		{
			y += 16;
			x = 0;
			continue;
		}
		
		if (Font_IsEUC(text[i]) && Font_IsEUC(text[i+1]))
		{
			code = ((unsigned char)text[i]<<8) | ((unsigned char)text[i+1]);
			code &= 0x7f7f;
			Font_PutKANJIEx(bgc , x , y-1 , code,1);
			Font_PutKANJIEx(bgc , x , y+1 , code,1);

			Font_PutKANJI(color , x , y , code);
			x += 16;
			i++;
		}
		else
		{
			if ((unsigned char)text[i] == 0x8e) i++;
			Font_PutCharEx(bgc , x , y-1 , text[i],1);
			Font_PutCharEx(bgc , x , y+1 , text[i],1);
			Font_PutChar(color , x , y , text[i]);
			x += 8;
		}
	}
}

void Font_PrintfEncoding(int encode,int color,int x,int y,const char *fmt,...)
{
	char buf[512];
	va_list arg;
	
	va_start(arg,fmt);
	vsprintf(buf,fmt,arg);

	va_end(arg);
	
	switch(encode)
	{
		case FONT_ENC_SJIS:
			Font_PutStrSJIS(color,x,y,buf);
		break;
		
		default:
		case FONT_ENC_EUC:
			Font_PutStrEUC(color,x,y,buf);
		break;
	}
}

void Font_Printf(int color,int x,int y,const char *fmt,...)
{
	char buf[512];
	va_list arg;
	
	va_start(arg,fmt);
	vsprintf(buf,fmt,arg);

	va_end(arg);
	
	if (flagSJIS)
		Font_PutStrSJIS(color,x,y,buf);
	else
		Font_PutStrEUC(color,x,y,buf);
}

