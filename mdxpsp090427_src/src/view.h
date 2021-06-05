#ifndef __VIEW_H__
#define __VIEW_H__

void view_Init(void);

void view_Start(void);
int  view_Display(void);
void view_KeyIn(int buttons);

#define VIEW_REL_BACK  0
#define VIEW_REL_ENTER 1

#endif

