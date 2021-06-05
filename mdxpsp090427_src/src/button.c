#include "button.h"

void button_Init(t_button *btn,int skip_current)
{
	btn->count  = 0;
	btn->level  = 0;
	btn->button = 0;
	btn->skip  = skip_current;
}

int button_Check(t_button *btn,int button)
{
	if (btn->button != button)
	{
		btn->level = 0;
		btn->count = 40; 
		btn->button = button;
		if (btn->skip)
		{
			btn->skip = 0;
			return 0;
		}
		return 1;
	}
	
	if (!button)
	{
		if (btn->skip) btn->skip = 0;
		return 0;
	}
	
	if (btn->count > 0)
	{
		btn->count--;
		return 0;
	}
	else
	{
		if (btn->level < 4)
		{
			btn->level++;
			btn->count = 3;
		}
		else
			btn->count = 0;

		return 1;
	}
	return 0;
}


