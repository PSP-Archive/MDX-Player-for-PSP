#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <psphprm.h>

#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "font.h"
#include "filer.h"

#include "pcm.h"
#include "popup.h"
#include "status.h"
#include "bg.h"
#include "player.h"
#include "menu.h"

#include "config.h"

#ifndef PROGNAME
#define PROGNAME "MDX PLAYER for PSP"
#endif

#ifndef VERNUM
#define VERNUM 0.00
#endif



PSP_MODULE_INFO("MDXPSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);


int exitCallback(int arg1, int arg2, void *common)
{
	graph_Term();
	sceKernelExitGame();
	return 0;
}

int callbackThread(SceSize args, void *argp) {
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (void*) exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks(void) {
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}

void set_title(void)
{
	char buf[256];
	
	sprintf(buf,"%s %.2f",PROGNAME,VERNUM);
	bg_SetCaption(buf);
}

int main(int argc,char *argv[])
{
	int i;
	int fgc;
	
	int task;


	
	SceCtrlData pad;
	u32 remkey;

	setupCallbacks();

	pspDebugScreenInit();

	status_InitTask();
	
	graph_Init();
	Font_Init();
	popup_Init();
	bg_Init();
	
	set_title();

	popup_SetNotice("Initializing...");
	bg_Display();
	popup_Display();
	graph_PutImage();
	graph_SwapBuffer();

	player_Init(argv[0]);
	menu_Init();
	pcm_init();
	
	filer_Init(argv[0]);
	

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);


	fgc = MAKE_RGB15(0x1F,0x1F,0x1F);

	task = TASK_PLAYER;
	status_CallTask(task);

	
	player_StartFiler();
	
	while(task != TASK_QUIT)
	{
		sceCtrlReadBufferPositive(&pad,1);
		sceHprmPeekCurrentKey(&remkey);
				
		player_CheckUpdate();

		task = status_GetCurrentTask();

		switch(task)
		{
			case TASK_FILER:
				filer_KeyIn(pad.Buttons);
				filer_RemIn(remkey);
			break;
			case TASK_POPUP:
				popup_KeyIn(pad.Buttons);
			break;
			case TASK_PLAYER:
				player_KeyIn(pad.Buttons);
				player_RemIn(remkey);
			break;
			case TASK_MENU:
				menu_KeyIn(pad.Buttons);
			break;
		}
		
		if (status_IsUpdateNeeded())
		{
			bg_Display();
			for( i = 0; i <= status_CurrentStackPos(); i++ )
			{
				if ( ! status_GetDisableUpdate( i ) )
				{
					switch( status_GetTaskID( i ) ) 
					{
						case TASK_FILER:
							filer_Display();
						break;
						case TASK_POPUP:
							popup_Display();
						break;
						case TASK_PLAYER:
							player_Display();
						break;
						case TASK_MENU:
							menu_Display();
						break;
					}
				}
			}

			graph_PutImage();
			graph_SwapBuffer();
			
			status_ClearUpdateNotify();
		}
		
	}
	
	graph_Term();
	sceKernelExitGame();

	return 0;
}
