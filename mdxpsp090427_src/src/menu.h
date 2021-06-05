#ifndef __MENU_H__
#define __MENU_H__

void menu_Init(void);

void menu_Start(void);
int  menu_Display(void);
void menu_KeyIn(int buttons);

#define MENU_REL_BACK  0
#define MENU_REL_ENTER 1

#endif

