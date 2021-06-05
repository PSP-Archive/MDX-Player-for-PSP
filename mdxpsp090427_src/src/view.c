#include <stdio.h>
#include <string.h>

#include <pspctrl.h>

#include "graph.h"
#include "font.h"
#include "button.h"

#include "view.h"

#include "status.h"

#define VIEW_MAX_LINE 15

static int view_data_pos = 0;
static int view_data_max = 0;

static t_button view_btndata;
static t_button view_remdata;

static char view_message[256];

void view_ResetButton(void)
{
	button_Init(&view_btndata,0);
	button_Init(&view_remdata,0);
}

void view_Init(void)
{
	view_data_pos = 0;
	view_data_max = 32;
	
	view_ResetButton();
	
	strcpy(view_message,"¡ß:BACK");
}

void view_Start(void)
{
	view_ResetButton();
}

void view_Up(void)
{
	if (view_data_pos > 0)
		view_data_pos--;
}


void view_Down(void)
{
	if ((view_data_pos + VIEW_MAX_LINE) <  view_data_max)
		view_data_pos++;
}

void view_Enter(void)
{
}

void view_Back(void)
{
	status_BackTask(VIEW_REL_BACK);
}

int view_Display(void)
{
	int  i;
	int  fgc  = MAKE_RGB15(0x1f,0x1f,0x1f);

	for(i = 0; i < VIEW_MAX_LINE && ( i + view_data_pos) < view_data_max; i++)
	{
		Font_Printf(fgc,16,16 * (i+1) , "%d", view_data_pos + i );
	}
	
	Font_Printf(fgc, 0 , SCR_HEIGHT - 16 , view_message );
	
	return 1;
}


void view_KeyIn(int buttons)
{
	if (button_Check(&view_btndata,buttons))
	{
		status_RiseUpdateNotify();

		if (buttons & PSP_CTRL_UP)
			view_Up();
			
		if (buttons & PSP_CTRL_DOWN)
			view_Down();

		if (buttons & PSP_CTRL_CIRCLE)
			view_Enter();
		
		if (buttons & PSP_CTRL_CROSS)
			view_Back();
	}
}