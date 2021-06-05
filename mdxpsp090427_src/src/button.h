#ifndef __BUTTON_H__
#define __BUTTON_H__

typedef struct
{
	int count;
	int step;
	int button;
	int level;
	int skip;
} t_button;

void button_Init (t_button *btn,int skip_current);
int  button_Check(t_button *btn,int button);

#endif
