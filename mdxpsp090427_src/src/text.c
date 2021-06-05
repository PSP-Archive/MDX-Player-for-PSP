#include <stdio.h>
#include <string.h>

#include <pspctrl.h>
#include <psphprm.h>

#include "graph.h"
#include "font.h"
#include "button.h"

#include "status.h"

int  text_cursor_y;
int  text_data_pos;
int  text_update_disp;
int  text_status;

void Text_Init(void)
{
	text_cursor_y = 0;
	text_data_pos = 0;
	text_update_disp = 0;
	text_status = STATUS_IDLE;
}

int Text_GetStatus(void)
{
	return text_status;
}

int Text_SetStatus(int status)
{
}

int  Text_IsUpdateDisplay(void)
{
	return text_update_disp;
}

