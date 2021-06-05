#include <stdio.h>
#include <string.h>

#include <pspctrl.h>

#include "graph.h"
#include "font.h"
#include "button.h"

#include "menu.h"

#include "popup.h"
#include "status.h"

#include "player.h"
#include "filer.h"

#define MENU_MAX_LINE (8)

#define MENU_H ((MENU_MAX_LINE+2) * 16)
#define MENU_Y (SCR_HEIGHT / 2) - (MENU_H / 2)

#define MENU_W (16 * 16)
#define MENU_X (SCR_WIDTH / 2) - (MENU_W / 2)

#define MENU_TYPE_BACK    0
#define MENU_TYPE_FILESEL 1
#define MENU_TYPE_MENU    2
#define MENU_TYPE_CMD     3

#define MENU_TYPE_DUMMY   127

#define MENU_CMD_NONE     0
#define MENU_CMD_SETPCM   1


typedef struct
{
	char *name;
	int  type;
	int  command;
	void *option;
} t_menu;

typedef struct
{
	int cursor;
	int data_pos;
	int data_max;
	t_menu *menu;
} t_menustat;

static t_menu menu_test_b[]=
{
 {"Back",MENU_TYPE_BACK,0,NULL},
 { NULL,0,0,NULL }
};

static t_menu menu_test_a[]=
{
 {"Test Menu B",MENU_TYPE_MENU,0,menu_test_b},
 {"Back",MENU_TYPE_BACK,0,NULL},
 {"表示テスト",MENU_TYPE_DUMMY,0,NULL},
 {"4",MENU_TYPE_DUMMY,0,NULL},
 {"5",MENU_TYPE_DUMMY,0,NULL},
 {"6",MENU_TYPE_DUMMY,0,NULL},
 {"7",MENU_TYPE_DUMMY,0,NULL},
 {"8",MENU_TYPE_DUMMY,0,NULL},
 {"9",MENU_TYPE_DUMMY,0,NULL},
 {"0",MENU_TYPE_DUMMY,0,NULL},
 {"1",MENU_TYPE_DUMMY,0,NULL},
 {"2",MENU_TYPE_DUMMY,0,NULL},

 
 { NULL,0,0,NULL }
};

static t_menu menu_root[]=
{
 {"Select PCM Dir",MENU_TYPE_CMD,MENU_CMD_SETPCM,NULL},
 {"Back",MENU_TYPE_BACK,0,NULL},

 { NULL,0,0,NULL }
};


#define MENU_STACK 16

static int menu_stackcnt = 0;
static t_menustat *current;
static t_menustat m_stack[MENU_STACK];

static t_button menu_btndata;
static t_button menu_remdata;

void menu_ResetButton(void)
{
	button_Init(&menu_btndata,0);
	button_Init(&menu_remdata,0);
}

void menu_ResetMenuStat(t_menu *menu)
{
	int i;
	
	current->cursor   = 0;
	current->data_pos = 0;
	
	for(i=0; menu[i].name; i++);
		
	current->data_max = i;
	current->menu = menu;
}

void menu_LoadMenu(t_menu *menu)
{
	if (menu_stackcnt >= MENU_STACK) 
		return;
	
	menu_stackcnt++;
	
	current = &m_stack[menu_stackcnt];

	menu_ResetMenuStat(menu);
}

int menu_BackMenu(void)
{
	if (menu_stackcnt <= 0)
		return -1;
		
	menu_stackcnt--;

	current = &m_stack[menu_stackcnt];
	
	return 0;
}

void menu_Init(void)
{
	current = &m_stack[0];
	menu_ResetMenuStat(menu_root);
	menu_ResetButton();
}

void menu_Start(void)
{
	menu_ResetButton();
}

void menu_Up(void)
{
	if (current->cursor > 0)
	{
		current->cursor--;
	}
	else
	{
		if (current->data_pos > 0)
			current->data_pos --;
	}
}


void menu_Down(void)
{
	if (current->cursor+1 < MENU_MAX_LINE && 
			(current->cursor + 1 + current->data_pos) < current->data_max)
		current->cursor++;
	else
	{
		if ((current->data_pos  + MENU_MAX_LINE) <  current->data_max)
			current->data_pos ++;
	}
}

void menu_Back(void)
{
	status_BackTask(MENU_REL_BACK);
}

void menu_Command(int command,void *option)
{
	char buf[2048];
	
	switch(command)
	{
		case MENU_CMD_SETPCM:
			filer_GetPath(buf,NULL);
			player_SetPCMdir(buf);
			popup_SetNotice("PCM directory is changed");
			status_CallTask(TASK_POPUP);
		break;
	}
}


void menu_Enter(void)
{
	t_menu *m_ptr;
	
	m_ptr = &current->menu[current->data_pos + current->cursor];
	
	if (m_ptr->type == 	MENU_TYPE_CMD)
	{
		menu_Command(m_ptr->command,m_ptr->option);
	}
	if (m_ptr->type == MENU_TYPE_MENU)
	{
		menu_LoadMenu((t_menu *)m_ptr->option);
	}
	if (m_ptr->type == MENU_TYPE_BACK)
	{
		if (menu_BackMenu())
			menu_Back();
	}
	
}


int menu_Display(void)
{
	int  i;
	int  fgc  = MAKE_RGB15(0x1f,0x1f,0x1f);
	
	graph_FillBox(MENU_X +5, MENU_Y +5 , MENU_W, MENU_H , MAKE_RGB15(0x00,0x00,0x05));
	graph_FillBox(MENU_X   , MENU_Y    , MENU_W, MENU_H , MAKE_RGB15(0x00,0x00,0x10));

	graph_FillBox(MENU_X + 16, MENU_Y + 16 + (16 * current->cursor) , MENU_W -32 , 16 , MAKE_RGB15(0x00,0x00,0x05));
	
	for(i = 0; i < MENU_MAX_LINE && ( i + current->data_pos) < current->data_max; i++)
	{
		Font_Printf(fgc,MENU_X + 16,MENU_Y + 16 +  (16 * i) , "%s", current->menu[current->data_pos + i].name );
	}

	return 1;
}


void menu_KeyIn(int buttons)
{
	if (button_Check(&menu_btndata,buttons))
	{
		status_RiseUpdateNotify();

		if (buttons & PSP_CTRL_UP)
			menu_Up();
			
		if (buttons & PSP_CTRL_DOWN)
			menu_Down();

		if (buttons & PSP_CTRL_CIRCLE)
			menu_Enter();
		
		if (buttons & PSP_CTRL_CROSS)
			menu_Back();
	}
}