#ifndef __POPUP_H__
#define __POPUP_H__

void popup_Init(void);
void popup_SetMessage(const char *message);
void popup_SetNotice(const char *message);

int  popup_Display(void);

void popup_KeyIn(int buttons);


#endif
