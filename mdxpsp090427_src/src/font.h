#ifndef __FONT_H__
#define __FONT_H__

#define FONT_ENC_SJIS 0
#define FONT_ENC_EUC  1

// 480/8  = 60
// 272/16 = 17
#define FONT_MAX_CHAR 60
#define FONT_MAX_LINE 17 

void Font_Init(void);
void Font_UseSJIS(int flag);

int Font_IsSJIS1(unsigned char ch);
int Font_IsSJIS2(unsigned char ch);
int Font_IsEUC(unsigned char ch);

unsigned short SJIS2JIS(unsigned short code);


void Font_PutKANJI(int color,int x,int y,int code);
void Font_PutChar(int color,int x,int y,int ch);

void Font_PutKANJIEx(int color,int x,int y,int code,int bold);
void Font_PutCharEx(int color,int x,int y,int ch,int bold);


void Font_PutStrSJIS(int color,int x,int y,const char *text);
void Font_PutStrEUC(int color,int x,int y,const char *text);
void Font_PrintfEncoding(int encode,int color,int x,int y,const char *fmt,...);
void Font_Printf(int color,int x,int y,const char *fmt,...);

#endif



