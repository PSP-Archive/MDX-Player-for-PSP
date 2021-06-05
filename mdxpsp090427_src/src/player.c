#include <psppower.h>

#include <stdio.h>
#include <string.h>
#include <pspctrl.h>
#include <psphprm.h>

#include "status.h"
#include "graph.h"
#include "font.h"
#include "button.h"

#include "pcm.h"
#include "player.h"
#include "filer.h"
#include "popup.h"

#include "menu.h"

#include "config.h"

#include "mdxmini.h"

#define MAX_MUSSIZE (64 * 1024)


// 5 * 16
#define PLAYER_HEIGHT 80
#define PLAYER_Y ((SCR_HEIGHT/2)-(PLAYER_HEIGHT/2))

static t_mdxmini mdx;


static t_button player_btndata;
static t_button player_remdata;

static unsigned char player_musdata[MAX_MUSSIZE];
static int player_mussize;

static char player_pcmdir[2048];

static char player_pwd[2048];
static char player_filename[256];

static char player_title[1024];
static char player_compo[256];


static char player_dbg[256];

static int player_fade;
static int player_count;

static int player_songlen;
static int player_songpos;

static int player_repeat;
static int player_repeat_sec;

static int player_loop_sec;


#define PLAYER_STOP 0
#define PLAYER_PLAY 1
#define PLAYER_PAUSE 2

static int player_flag;

static unsigned player_oldtick;
static unsigned player_curtick;
static unsigned player_tps;

void player_SetPCMdir(char *pcmdir)
{
	char *pcmpath[3];
	
	if (player_pcmdir != pcmdir)
		strcpy(player_pcmdir,pcmdir);
	
	pcmpath[0] = player_pcmdir;
	pcmpath[1] = NULL;


//	loadrhythmsample(pcmdir);
//	setpcmdir(pcmpath);
//	setpcmrate(SOUND_55K);
}

void player_SetMUSdir(char *musdir)
{
	strcpy(player_pwd,musdir);
}


void player_LoadSetting(void)
{
	config_SetStr(CONFIG_ID_MUSDIR,player_pwd);
	config_SetStr(CONFIG_ID_PCMDIR,player_pcmdir);
	config_SetStr(CONFIG_ID_MUSFILE,player_filename);
	config_SetInt(CONFIG_ID_REPEAT,player_repeat);

	
	config_ReadFile();
	
	config_GetStr(CONFIG_ID_MUSDIR,player_pwd);
	config_GetStr(CONFIG_ID_PCMDIR,player_pcmdir);
	config_GetStr(CONFIG_ID_MUSFILE,player_filename);
	player_repeat = config_GetInt(CONFIG_ID_REPEAT);

}

void player_SaveSetting(void)
{
	config_SetStr(CONFIG_ID_MUSDIR,player_pwd);
	config_SetStr(CONFIG_ID_PCMDIR,player_pcmdir);
	config_SetStr(CONFIG_ID_MUSFILE,player_filename);
	config_SetInt(CONFIG_ID_REPEAT,player_repeat);
	
	config_WriteFile();
}

void player_Init(char *execPath)
{
	
	player_count = 0;
	player_fade  = 0;

	player_dbg[0] = 0;
	
	player_pwd[0] = 0;
	player_filename[0] = 0;

	player_title[0] = 0;
	player_compo[0] = 0;
	
	player_songlen = 300;
	player_songpos = 0;
	
	player_mussize = 0;
	
	player_repeat = 0;
	player_repeat_sec = 0;
	player_loop_sec = 0;
	
	player_flag = PLAYER_STOP;

	button_Init(&player_btndata,1);
	button_Init(&player_remdata,1);
	
	config_Init();
	
	config_SetConfigFilePath(execPath);
	config_SetConfigFileName("MDXPLAY.CFG");
	
	config_CutLastItem(player_pwd,execPath);
	config_CutLastItem(player_pcmdir,execPath);
	
	player_LoadSetting();
	
//	pmdwininit(player_pcmdir);

	player_SetPCMdir(player_pcmdir);

}

void player_ResetButton(void)
{
	button_Init(&player_btndata,1);
	button_Init(&player_remdata,1);
}

void player_RepeatMode(void)
{
	if (player_repeat < 3)
		player_repeat++;
	else
		player_repeat = 0;
	
	player_repeat_sec = player_loop_sec * player_repeat;
	status_RiseUpdateNotify();
}

void player_SetCPU_Low(void)
{
	scePowerSetClockFrequency(222, 222, 111);
}

void player_SetCPU_High(void)
{
	scePowerSetClockFrequency(333, 333, 166);
}


// 0 .. OK
int player_LoadFile(char *file)
{
	return mdx_open(&mdx,file);
}

int player_PlayFile(const char *dir,const char *file)
{
	int ret;

	int loop;
	int length;
	
	char playfile[2048 + 256];
	
	player_SetCPU_High();
	
	strcpy( player_filename , file );
	strcpy( player_pwd , dir );

	strcpy( playfile , dir );
	strcat( playfile , "/" );
	strcat( playfile , file );


	player_tps = status_GetTickPerSec();
	player_curtick = status_GetTick();
	player_oldtick = player_curtick;


	ret = player_LoadFile(playfile);

	if (ret)
	{
		player_SetCPU_Low();
		return 0;
	}
	
	loop = 0;
	length = mdx_get_length(&mdx) * 1000;

	if (!loop)
		player_loop_sec = 0;
	else
		player_loop_sec = loop / 1000;
		
	player_title[0] = 0;
	player_compo[0] = 0;

	mdx_get_title(&mdx,player_title);
//	getmemo3(player_title,player_musdata,player_mussize,1);
//	getmemo3(player_compo,player_musdata,player_mussize,2);

	player_SetSongLen(length/1000);

	if (!ret)
		sprintf(player_dbg,"¢¤:BACK ¡û:PAUSE R:REPEAT [%d]",player_count);
	else
		sprintf(player_dbg,"ret = %d [%d]",ret,player_count);
		
	player_count++;
	player_fade = 0;
	
	pcm_clearTime();
	pcm_start();
	
	player_ResetButton();
	
	status_RiseUpdateNotify();
	player_flag = PLAYER_PLAY;
	
	return 1;
}

void player_StartFiler(void)
{
	filer_SetPath(player_pwd,player_filename);
	filer_ReloadDir();

	filer_SetMessage("¡û:PLAY ¢¤:DIR UP ¡ß:QUIT");
	status_CallTask(TASK_FILER);
}

void player_Pause(void)
{
	if (player_flag == PLAYER_STOP)
		return;
	
	if (player_flag != PLAYER_PAUSE)
	{
		player_flag = PLAYER_PAUSE;
		pcm_stop();
	}
	else
	{
		player_flag = PLAYER_PLAY;
		pcm_start();
	}
}

void player_Stop(void)
{
	if (player_flag == PLAYER_STOP)
		return;
	
	player_flag = PLAYER_STOP;
	pcm_stop();
	pcm_clearBuffer();
		
	mdx_stop(&mdx);

	player_SetCPU_Low();

//	music_stop();
}

void player_NextSong(int foward)
{
	int count;
	
	char old_pwd[2048];
	char old_file[256];
	
	player_Stop();
	
	filer_GetPath(old_pwd,old_file);
	
	filer_SetPath(player_pwd,player_filename);
	filer_ReloadDir();
	
	count = filer_SearchNextFile(foward,0);
	while(count)
	{
		filer_GetPath(player_pwd,player_filename);
		if (player_PlayFile(player_pwd,player_filename))
			break;
				
		count = filer_SearchNextFile(foward,count);
	}
	
	filer_SetPath(old_pwd,old_file);
	filer_ReloadDir();

}

void player_CheckUpdate(void)
{
	short buf[PCM_BLOCKSIZE * PCM_CH];
	int i;
	int time;
	
	if (player_flag != PLAYER_PLAY)
		return;
	
	for ( i = 0; i < 4;  i++)
	{
		if (!pcm_chkEmpty(PCM_BLOCKSIZE))
			break;
		
		mdx_calc_sample(&mdx , buf , PCM_BLOCKSIZE );
		// getpcmdata(buf,PCM_BLOCKSIZE);
		
		pcm_setBuffer(buf,PCM_BLOCKSIZE);
	}
		
	time = pcm_getTime();
	if (player_songpos != time)
	{
		player_songpos = time;
		if (player_fade)
		{
			if (player_songpos >= (player_songlen))
			{
				player_NextSong(1);
			}
		
		}
		else
		{
			if ((player_repeat_sec == 0 || player_repeat != 3) && 
				player_songpos >= (player_songlen + player_repeat_sec))
			{
				player_fade = 1;
				// fadeout2(5000);
			}
		}
		
		status_RiseUpdateNotify();
	}
}

void player_SetSongLen(int len)
{
	player_songlen = len;
	status_RiseUpdateNotify();
}

void player_SetSongPos(int pos)
{
	player_songpos = pos;
	status_RiseUpdateNotify();
}

int player_Display(void)
{
	int i;
	int len;

	int player_x;
	int player_width;

	int fgc  = MAKE_RGB15(0x1f,0x1f,0x1f);

	char file[128];
	char title[128];
	char compo[128];
	
	char info[128];
	
	
	
	if (player_flag == PLAYER_STOP)
		return 0;

	sprintf(file ,"File     : %.64s",player_filename);
	sprintf(title,"Title    : %.128s",player_title);
	sprintf(compo,"Composer : %.64s",player_compo);

	if (player_repeat == 3)
	{
		sprintf(info, "Time     : %02d:%02d / --:--",
			player_songpos / 60 , player_songpos % 60 );
	}
	else
	{
		len = player_songlen + player_repeat_sec;
		sprintf(info, "Time     : %02d:%02d / %02d:%02d",
			player_songpos / 60 , player_songpos % 60 ,
			len / 60 , len % 60 );
	}
	len = strlen( file );
	
	i = strlen(title);
	if (i > len) len = i;
	
	i = strlen(compo);
	if (i > len) len = i;

	i = strlen(info);
	if (i > len) len = i;
	
	player_width = (len+2) * 8;

	player_x = 16;

//	(SCR_WIDTH/2) - (player_width / 2);
		
//	graph_FillBox(player_x +5, PLAYER_Y +5 , player_width, PLAYER_HEIGHT , MAKE_RGB15(0x00,0x00,0x05));
//	graph_FillBox(player_x , PLAYER_Y , player_width, PLAYER_HEIGHT , MAKE_RGB15(0x00,0x00,0x10));

//	graph_FillBox(0 , SCR_HEIGHT-16 , SCR_WIDTH, 16 , MAKE_RGB15(0x00,0x00,0x10));

	Font_PrintfEncoding(FONT_ENC_SJIS,fgc, player_x, PLAYER_Y  , file );
	Font_PrintfEncoding(FONT_ENC_SJIS,fgc, player_x, PLAYER_Y + 16 , title );
	//Font_PrintfEncoding(FONT_ENC_SJIS,fgc, player_x, PLAYER_Y + 32 , compo );
	Font_PrintfEncoding(FONT_ENC_SJIS,fgc, player_x , PLAYER_Y + 64 , info  );

	Font_Printf(fgc, 0 , SCR_HEIGHT - 16 , player_dbg );


	return 1;
}

void player_KeyIn(int buttons)
{
	if (status_IsTaskBack())
	{
		switch(status_GetPreviousTask())
		{
			case TASK_MENU:
				player_SaveSetting();
				status_DisableUpdate(0);
			break;
			case TASK_POPUP:
				if (status_GetResult())
				{
					player_SaveSetting();
					status_CallTask(TASK_QUIT);
				}
				else
				{
					player_StartFiler();
				}
			break;
			case TASK_FILER:
				switch(status_GetResult())
				{
					case FILER_REL_CANCEL:
						popup_SetMessage("QUIT NOW?");
						status_CallTask(TASK_POPUP);
					break;
					case FILER_REL_SELECT:
						filer_GetPath(player_pwd,player_filename);
						player_PlayFile(player_pwd,player_filename);
					break;
				}
			break;
		}
				
		player_ResetButton();
		status_ClearTaskNotify();
		return;
	}


	if (button_Check(&player_btndata,buttons))
	{
		if (buttons & PSP_CTRL_TRIANGLE)
		{
			player_Stop();
			player_StartFiler();
			return;
		}
		
		if (buttons & PSP_CTRL_SELECT)
		{
			menu_Start();
			status_DisableUpdate(0);
			status_CallTask(TASK_MENU);
			return;
		}
		
		if (buttons & PSP_CTRL_DOWN)
		{
			player_NextSong(1);
			return;
		}
		if (buttons & PSP_CTRL_UP)
		{
			player_NextSong(0);
			return;
		}
		
		if (buttons & PSP_CTRL_RTRIGGER)
		{
			player_RepeatMode();
			return;
		}
				
		if (buttons & PSP_CTRL_CIRCLE)
		{
			player_Pause();
			return;
		}
	}
}

void player_RemIn(int buttons)
{
	if (status_IsTaskBack())
	{
		player_ResetButton();
		return;
	}
	
	if (button_Check(&player_remdata,buttons))
	{
		if (!(buttons & PSP_HPRM_HOLD))
			status_RiseUpdateNotify();

		if (buttons & PSP_HPRM_PLAYPAUSE)
			player_Pause();

		if (buttons & PSP_HPRM_FORWARD)
			player_NextSong(1);
		
		if (buttons & PSP_HPRM_BACK)
			player_NextSong(0);
	}
}
