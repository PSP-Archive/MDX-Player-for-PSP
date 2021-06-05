#include <string.h>

#include "graph.h"
#include "font.h"

#include "status.h"

static char bg_caption[128];

void bg_Init(void)
{
	bg_caption[0] = 0;
}

void bg_SetCaption(const char *data)
{
	strcpy(bg_caption,data);
}

int bg_Display(void)
{
	int  pos;
	int  batt;
	
	int  white_c  = MAKE_RGB15(0x1f,0x1f,0x1f);
	int  orenge_c = MAKE_RGB15(0x1f,0x1f,0x00);
	int  red_c    = MAKE_RGB15(0x1f,0x00,0x00);
	
	char buf[128];

	graph_FillGrad(0, 0 ,SCR_WIDTH,12,MAKE_RGB15(0x8,0x08,0x08),MAKE_RGB15(0x10,0x10,0x10));
	graph_FillGrad(0, 12,SCR_WIDTH,4,MAKE_RGB15(0x8,0x08,0x08),MAKE_RGB15(0x5,0x05,0x05));
	graph_FillBox (0,16 ,SCR_WIDTH,220,MAKE_RGB15(0,0,0x15));
	graph_FillGrad(0,236,SCR_WIDTH,SCR_HEIGHT-236,MAKE_RGB15(0,0,0x15),MAKE_RGB15(0,0,0x03));
	
	Font_Printf(white_c,0,0,bg_caption);

	status_GetBatteryString(buf);

	batt = status_GetBatteryPercent();
	pos  = strlen(buf);

	if (batt < 30)
		Font_Printf(red_c,(FONT_MAX_CHAR - pos)*8 , 0 ,"%s",buf);
	else
	if (batt < 50)
		Font_Printf(orenge_c,(FONT_MAX_CHAR - pos)*8 , 0 ,"%s",buf);
	else
		Font_Printf(white_c,(FONT_MAX_CHAR - pos)*8 , 0 ,"%s",buf);
	
	status_GetTimeString(buf);
	pos  = pos + strlen(buf) + 1;
	
	Font_Printf(white_c,(FONT_MAX_CHAR - pos)*8 , 0 ,"%s",buf);

	return 1;
}
