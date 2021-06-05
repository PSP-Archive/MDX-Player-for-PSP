#include <stdio.h>
#include <string.h>
#include <pspctrl.h>

#include "status.h"
#include "graph.h"
#include "font.h"
#include "button.h"

// 5 * 16
#define POPUP_HEIGHT 80 
#define POPUP_Y ((SCR_HEIGHT/2)-(POPUP_HEIGHT/2))

static char popup_message[256];
static int popup_notice;

static t_button popup_btndata;

void popup_Init(void)
{
	popup_notice = 0;
	button_Init(&popup_btndata,1);
}

void popup_SetMessage(const char *message)
{
	popup_notice = 0;
	strcpy( popup_message , message );
	button_Init(&popup_btndata,1);
	status_RiseUpdateNotify();
}
void popup_SetNotice(const char *message)
{
	popup_notice = 1;
	strcpy( popup_message , message );
	button_Init(&popup_btndata,1);
	status_RiseUpdateNotify();
}



int popup_Display(void)
{
	int len;
	int info_len;

	int info_x;
	int msg_x;

	int popup_x;
	int popup_width;

	int fgc  = MAKE_RGB15(0x1f,0x1f,0x1f);

	char *info = "¡û:OK ¡ß:CANCEL";
	
	len = strlen( popup_message );
	info_len = strlen( info );
	
	if (len < info_len)
		len = info_len;
	
	popup_width = (len+2) * 8;

	popup_x = (SCR_WIDTH/2) - (popup_width/2);
	msg_x   = (SCR_WIDTH/2) - ((strlen( popup_message ) * 8) / 2); 
	info_x  = (SCR_WIDTH/2) - ((info_len*8)/2);
		
	graph_FillBox(popup_x +5, POPUP_Y +5 , popup_width, POPUP_HEIGHT , MAKE_RGB15(0x00,0x00,0x05));
	graph_FillBox(popup_x , POPUP_Y , popup_width, POPUP_HEIGHT , MAKE_RGB15(0x00,0x00,0x10));
	
	if (popup_notice)
	{
		Font_Printf(fgc,  msg_x, POPUP_Y + 32 , popup_message );
	}
	else
	{
		Font_Printf(fgc,  msg_x, POPUP_Y + 16 , popup_message );
		Font_Printf(fgc, info_x, POPUP_Y + 48 , info );
	}
	return 1;
}

void popup_KeyIn(int buttons)
{
	if (button_Check(&popup_btndata,buttons))
	{
		if (buttons & PSP_CTRL_CIRCLE)
		{
			status_BackTask(1);
		}
		if (buttons & PSP_CTRL_CROSS)
		{
			status_BackTask(0);
		}
	}
}
