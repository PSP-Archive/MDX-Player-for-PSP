#include <pspgu.h>
#include <pspdisplay.h>
#include <pspkernel.h>

#include "graph.h"

static unsigned int   __attribute__((aligned(16))) list[262144];

static unsigned short __attribute__((aligned(16))) pixels[BUF_WIDTH * SCR_HEIGHT];

typedef struct
{
	unsigned short u, v;
	unsigned short color;
	short x, y, z;
} t_vertex;

void graph_Init(void)
{
	sceGuInit();

	sceGuStart(GU_DIRECT,list);

	// set buffers
	sceGuDrawBuffer(GU_PSM_5551,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)RGB_WORD_VRAMSIZE,BUF_WIDTH);
	sceGuDepthBuffer((void*)RGB_WORD_VRAMSIZE+RGB_WORD_VRAMSIZE,BUF_WIDTH);
	
	// set graphic port
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDisable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);

	// clear frame
	sceGuClearColor(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	sceGuFinish();
	sceGuSync(0,0);
	
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	sceKernelDcacheWritebackAll();

}

void graph_Term(void)
{
	sceGuTerm();
}

void graph_Blit(int x,int y,int w,int h,int dx,int dy)
{
	int width;
	int start,end;
	
	for(start = x , end = x + w; start < end; start += GU_SLICE,dx+=GU_SLICE)
	{
	
		if ((start + GU_SLICE) < end) 
			width = GU_SLICE;
		else
			width = end - start;
			
		t_vertex *vert= (t_vertex*)sceGuGetMemory(2 * sizeof(t_vertex));
	
		vert[0].u = start; vert[0].v = y;
		vert[0].color = 0;
		vert[0].x = dx; vert[0].y = dy; vert[0].z = 0;
		
		vert[1].u = start + width; vert[1].v = y + h;
		vert[1].color = 0;
		vert[1].x = dx + width; vert[1].y = dy + h; vert[1].z = 0;

		sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_COLOR_5551|GU_VERTEX_16BIT|GU_TRANSFORM_2D,2,0,vert);
	}
}


/* (x,y)-(x+w,y+h) -> (dx,dy) */
void graph_PutTexture(int x,int y,int w,int h,int dx,int dy)
{

	t_vertex *vert= (t_vertex*)sceGuGetMemory(2 * sizeof(t_vertex));
	
	vert[0].u = x; vert[0].v = y;
	vert[0].color = 0;
	vert[0].x = dx; vert[0].y = dy; vert[0].z = 0;
		
	vert[1].u = x+w; vert[1].v = y+h;
	vert[1].color = 0;
	vert[1].x = dx + w; vert[1].y = dy+h; vert[1].z = 0;

	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_COLOR_5551|GU_VERTEX_16BIT|GU_TRANSFORM_2D,2,0,vert);
}



void graph_FillClr(int x,int y,int w,int h,int dx,int dy,int clr)
{
	
	t_vertex *vert= (t_vertex*)sceGuGetMemory(2 * sizeof(t_vertex));
	
	vert[0].u = x; vert[0].v = y;
	vert[0].color = clr;
	vert[0].x = dx; vert[0].y = dy; vert[0].z = 0;
		
	vert[1].u = x+w; vert[1].v = y+h;
	vert[1].color = clr;
	vert[1].x = dx+ w; vert[1].y = dy+h; vert[1].z = 0;

	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_COLOR_5551|GU_VERTEX_16BIT|GU_TRANSFORM_2D,2,0,vert);
}


void graph_FillClr32(int x,int y,int w,int h,int dx,int dy,int clr)
{
	
	t_vertex *vert= (t_vertex*)sceGuGetMemory(2 * sizeof(t_vertex));
	
	vert[0].u = x; vert[0].v = y;
	vert[0].color = clr;
	vert[0].x = dx; vert[0].y = dy; vert[0].z = 0;
		
	vert[1].u = x+w; vert[1].v = y+h;
	vert[1].color = clr;
	vert[1].x = dx+ w; vert[1].y = dy+h; vert[1].z = 0;

	sceGuDrawArray(GU_SPRITES,GU_TEXTURE_16BIT|GU_COLOR_8888|GU_VERTEX_16BIT|GU_TRANSFORM_2D,2,0,vert);
}


void graph_UseTexture(int flag)
{
	if (flag)
		sceGuEnable(GU_TEXTURE_2D);
	else
		sceGuDisable(GU_TEXTURE_2D);
}

void graph_UseAlpha(int flag)
{
	if (flag)
		sceGuEnable(GU_ALPHA_TEST);
	else
		sceGuDisable(GU_ALPHA_TEST);
}


void graph_SetTexture(void *texture,int width)
{
		sceGuTexMode(GU_PSM_5551,0,0,0);
		sceGuTexImage(0,width,width,width,texture);
		sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
		sceGuTexFilter(GU_NEAREST,GU_NEAREST);
}

void graph_FillPixBuf(int color)
{
	int i;
	for(i = 0; i < BUF_WIDTH * SCR_HEIGHT; i++)
		pixels[i] = color;
}

void graph_FillBox(int x,int y,int w,int h,int color)
{
	int i,j,ypos;
	
	for(j = y; j < y + h; j++)
	{
		ypos = (BUF_WIDTH * j);
		
		for(i = x; i < x + w; i++)
		{
			pixels[ypos + i] = color;
		}
	}
}

void graph_FillGrad(int x,int y,int w,int h,int cl1,int cl2)
{
	int i,j,ypos;

	int color;

	int rs,gs,bs;
	int re,ge,be;
	
	rs = cl1&0x1f;
	gs = (cl1>>5)&0x1f;
	bs = (cl1>>10)&0x1f;

	re = cl2&0x1f;
	ge = (cl2>>5)&0x1f;
	be = (cl2>>10)&0x1f;

	for(j = y; j < y + h; j++)
	{
		ypos = (BUF_WIDTH * j);
		color = MAKE_RGB15(
			rs + ((re-rs)*(j-y))/h , 
			gs + ((ge-gs)*(j-y))/h , 
			bs + ((be-bs)*(j-y))/h);

		for(i = x; i < x + w; i++)
		{
			pixels[ypos + i] = color;
		}
	}
}

void graph_ClearPixBuf(void)
{
	graph_FillPixBuf(0);
}

void graph_FillTestPattern(void)
{
	int i;
	for(i = 0; i < BUF_WIDTH * SCR_HEIGHT; i++)
		pixels[i] = i & 0xffff;
}


void graph_Start(void)
{
	sceGuStart(GU_DIRECT,list);
}

void graph_Finish(void)
{
	sceGuFinish();
	sceGuSync(0,0);
}

void graph_WaitDone(void)
{
	sceGuSync(0,2);
}


void *graph_GetPixelArray(void)
{
	return pixels;
}

void graph_PutImage(void)
{
	sceKernelDcacheWritebackAll();

	graph_Start();
	graph_UseTexture(1);
	graph_SetTexture(pixels,512);
	graph_Blit(0,0,SCR_WIDTH,SCR_HEIGHT,0,0);
	graph_Finish();
}

void graph_WaitVSYNC(void)
{
	sceDisplayWaitVblank();
}

void graph_SwapBuffer(void)
{
	unsigned char *fb;
	fb = sceGuSwapBuffers();
}
