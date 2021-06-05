#ifndef __GRAPH_H__
#define __GRAPH_H__

#define RGB_WORD_VRAMSIZE 0x44000
#define RGB_LONG_VRAMSIZE 0x88000

#define BUF_WIDTH  512
#define SCR_WIDTH  480
#define SCR_HEIGHT 272
#define GU_SLICE   32

#define MAKE_RGB15(r,g,b) ((r)&0x1f)|(((g)&0x1f)<<5)|(((b)&0x1f)<<10)
#define MAKE_RGB15A(r,g,b,a) ((r)&0x1f)|(((g)&0x1f)<<5)|(((b)&0x1f)<<10)|(((a)&0x01)<<15)

void graph_Init(void);
void graph_Term(void);


/* (x,y)-(x+w,y+h) -> (dx,dy) */
void graph_Blit(int x,int y,int w,int h,int dx,int dy);

/* (x,y)-(x+w,y+h) -> (dx,dy) */
void graph_PutTexture(int x,int y,int w,int h,int dx,int dy);


void graph_FillClr(int x,int y,int w,int h,int dx,int dy,int clr);
void graph_FillClr32(int x,int y,int w,int h,int dx,int dy,int clr);
void graph_FillBox(int x,int y,int w,int h,int color);
void graph_FillGrad(int x,int y,int w,int h,int cl1,int cl2);


void graph_UseTexture(int flag);
void graph_UseAlpha(int flag);

void graph_SetTexture(void *texture,int width);

void graph_ClearPixBuf(void);
void graph_FillPixBuf(int color);

void graph_FillTestPattern(void);

void graph_Start(void);
void graph_Finish(void);

void *graph_GetPixelArray(void);
void graph_PutImage(void);

void graph_WaitDone(void);
void graph_WaitVSYNC(void);
void graph_SwapBuffer(void);

#endif
