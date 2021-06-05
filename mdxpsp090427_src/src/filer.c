#include <stdio.h>
#include <string.h>

#include <pspctrl.h>
#include <psphprm.h>

#include <pspiofilemgr.h>

#include "graph.h"
#include "font.h"
#include "button.h"

#include "filer.h"

#include "popup.h"
#include "status.h"

#include "menu.h"

#define FILER_MAX_LINE 13 
#define FILER_MAX_ENTRY 1024

static char filer_pwd[2048];
static char filer_filename[256];

static int  filer_cursor;
static int  filer_cursor_y;
static int  filer_cursor_cnt;

static int  filer_data_pos;
static int  filer_data_max;


static t_button filer_btndata;
static t_button filer_remdata;

static SceIoDirent filer_filelist[FILER_MAX_ENTRY];

static SceIoDirent *filer_sorted_list[FILER_MAX_ENTRY];

static int filer_sorted_max;

static int 	filer_NextFrame;
static int  filer_CurrentTick;

static char filer_BlinkArray[] =
{0x08,0x07,0x06,0x05,0x06,0x07,0x08,0x09};

static char filer_msg[128];

static void filer_DirectoryUp(char *path,char *file)
{
	int i;
	
	for(i = strlen(path); i > 0; i--)
		if (path[i] == '/')
		{
			path[i] = 0;
			if (file)
			{
				strcpy( file , path + i + 1 );
			}
			
			break;
		}
}

static void filer_DirectoryDown(char *path,char *element)
{
	strcat(path,"/");
	strcat(path,element);
}

void filer_SortEntry(void)
{
	int i,j,k;
	int top,prev;

	char *ext;
	SceIoDirent *entry,*p_ent;

	int chain[FILER_MAX_ENTRY];

	top = -1;
	
	for(i=j=0; i < filer_data_max; i++)
	{
		entry = &filer_filelist[i];
		
		ext = strrchr(entry->d_name,'.');
		
		if (ext)
		{
			if (!(entry->d_stat.st_mode & FIO_S_IFDIR) && 
				entry->d_name[0] == '.')
					ext = NULL;
			else
			if (strnicmp(ext+1,"mdx",3) != 0)
					ext = NULL;
		}
		
		if ((entry->d_stat.st_mode & FIO_S_IFDIR) || ext)
		{
			chain[i] = -1;
			prev = -1;
			
			k = top;
			
			if (top < 0)
				top = i;

			while(k >= 0)
			{
				p_ent = &filer_filelist[k];
				if ( strcmp( p_ent->d_name , entry->d_name) < 0)
				{
					if (chain[k] < 0)
					{
						chain[k] = i;
						break;
					}
					else
					{
						prev = k;
						k = chain[k];
					}
				}
				else
				{
					if (prev < 0)
					{
						top = i;
						chain[i] = k;
					}
					else
					{
						chain[i] = chain[prev];
						chain[prev] = i;
					}
					break;
				}
			}
			
		}
	}
	
	k = top;
	i = 0;
			
	while(k >= 0)
	{
		filer_sorted_list[i++] = &filer_filelist[k];
		k = chain[k];
	}
	
	filer_sorted_max = i;

}

void filer_ReadDirectory(void)
{
	int i;
	int fd;
	
	filer_data_pos = 0;
	
	fd = sceIoDopen(filer_pwd);
	
	if (fd >= 0)
	{
		for(i = 0; i < FILER_MAX_ENTRY; i++)
		{
			if (sceIoDread(fd,&filer_filelist[i]) <= 0) break;
		}
		filer_data_max = i;
		sceIoDclose(fd);
		filer_SortEntry();
	}
}

static void filer_ResetButton(int skip_current)
{
	button_Init(&filer_btndata,skip_current);
	button_Init(&filer_remdata,skip_current);
}

void filer_Init(const char *path)
{
	filer_data_pos = 0;
	filer_data_max = 50;

	filer_ResetButton(0);

	filer_cursor     = 1;
	filer_cursor_y   = 0;
	filer_cursor_cnt = 0;

	strcpy(filer_pwd,path);
	
	filer_msg[0] = 0;

	filer_DirectoryUp(filer_pwd,filer_filename);
	filer_ReadDirectory();
	
	filer_NextFrame = status_GetTickPerSec() / 4;
	filer_CurrentTick = status_GetTick();
}

void filer_SetMessage(char *msg)
{
	strcpy(filer_msg,msg);
}

void filer_GetPath(char *pwd,char *file)
{
	if (pwd)
		strcpy(pwd,filer_pwd);
	
	if (file)
		strcpy(file,filer_filename);
}

void filer_SetPath(const char *pwd,const char *file)
{
	if (pwd)
		strcpy(filer_pwd,pwd);
	
	if (file)
		strcpy(filer_filename,file);
}

void filer_StringLimit(char *dest,const char *src,int len)
{
	int i;
	
	i = strlen(src);
	
	if (len < i)
		i = len;

	strncpy(dest,src,i);

	dest[i]   = 0;
	dest[i+1] = 0;
}

int filer_MenuUp(void)
{
	if (filer_cursor_y > 0)
		filer_cursor_y --;
	else
	{
		if (filer_data_pos > 0)
			filer_data_pos--;
		else
			return 0;
	}
	return 1;
}

int filer_MenuDown(void)
{
	if (filer_cursor_y < FILER_MAX_LINE && filer_cursor_y + 1 < filer_sorted_max)
	{
		filer_cursor_y ++;
	}
	else
	{
		if (filer_data_pos + FILER_MAX_LINE + 1 < filer_sorted_max)
			filer_data_pos++;
		else
			return 0;
	}
	return 1;
}

void filer_SetCursorPos(int newpos)
{
	int i;
	int oldpos;
	
	oldpos = filer_cursor_y + filer_data_pos;

	if (oldpos > newpos)
	{
		for(i=0; i < (oldpos - newpos); i++)
			filer_MenuUp();
	}
	else
	{
		for(i=0; i < (newpos - oldpos); i++)
			filer_MenuDown();
	}
}


void filer_SearchPos(const char *filename)
{
	int i;
	
	filer_cursor_y = 0;
	filer_data_pos = 0;
	
	for( i = 0; i < filer_sorted_max; i++ )
	{
		if (strcmp(filer_sorted_list[i]->d_name , filename) == 0)
		{
			filer_SetCursorPos(i);
			break;
		}
	}
}

int filer_Display(void)
{
	int  i;
	int  col;
	int  fgc  = MAKE_RGB15(0x1f,0x1f,0x1f);
	int  dirc = MAKE_RGB15(0x00,0x1f,0x1f);
	char buf[128];


	SceIoDirent *entry;
	
	filer_StringLimit(buf , filer_pwd , 48);
	Font_PrintfEncoding(FONT_ENC_SJIS,
				fgc,0,16,"[%s] %d file(s)" , buf , filer_sorted_max);
	
	if (filer_cursor)
	{
		col = MAKE_RGB15(0,0,filer_BlinkArray[filer_cursor_cnt]);
		graph_FillBox(16,16 * (filer_cursor_y + 2),SCR_WIDTH-32,16,col);
	}
	
	for(i = 0; i <= FILER_MAX_LINE && filer_data_pos + i < filer_sorted_max; i++)
	{
		entry = filer_sorted_list[i + filer_data_pos];
		if (entry->d_stat.st_mode & FIO_S_IFDIR)
		{
			filer_StringLimit(buf , entry->d_name , FONT_MAX_CHAR - 6);
			Font_PrintfEncoding(FONT_ENC_SJIS,dirc,16,16 * (i+2) , "<%s>", buf);
		}
		else
		{
			filer_StringLimit(buf , entry->d_name , FONT_MAX_CHAR - 4);
			Font_PrintfEncoding(FONT_ENC_SJIS,fgc,16,16 * (i+2) , "%s", buf);
		}
	}

		entry = filer_sorted_list[filer_data_pos + filer_cursor_y];
	i = strcmp(entry->d_name , filer_filename );
	
	Font_Printf(fgc,0,16 * 16 ,filer_msg);
		
	return 1;
}

int  filer_NextFile(int max)
{
	SceIoDirent *entry;
	int i;
	int pos = filer_data_pos + filer_cursor_y;
	
	for(i = 0; i < max; i++)
	{
		pos++;
		if (pos >= filer_sorted_max)
			pos = 0;
	
		entry = filer_sorted_list[pos];
		if (!(entry->d_stat.st_mode & FIO_S_IFDIR))
		{
				filer_SetCursorPos(pos);
				return max-i;
		}
	}
	
	return 0;
}

int  filer_PrevFile(int max)
{
	SceIoDirent *entry;
	int i;
	int pos = filer_data_pos + filer_cursor_y;
	
	for(i = 0; i < max; i++)
	{
		pos--;
		if (pos < 0)
			pos = filer_sorted_max-1;
	
		entry = filer_sorted_list[pos];
		if (!(entry->d_stat.st_mode & FIO_S_IFDIR))
		{
				filer_SetCursorPos(pos);
				return max-i;
		}
	}
	
	return 0;
}

static void filer_MenuDirUp(void)
{
	filer_DirectoryUp(filer_pwd,filer_filename);
	filer_ReadDirectory();
	filer_SearchPos(filer_filename);
}

void filer_ReloadDir(void)
{
	filer_ReadDirectory();
	filer_SearchPos(filer_filename);
}

int filer_SearchNextFile(int forward,int count)
{
	SceIoDirent *entry;
	
	if (!count)
		count = filer_sorted_max;
		
	while(count) 
	{
		if (forward)
				count = filer_NextFile(count);
		else
				count = filer_PrevFile(count);
		
		if (!count) 
			break;
			
		entry = filer_sorted_list[filer_data_pos + filer_cursor_y];
		
		strcpy(filer_filename,entry->d_name);
		
		return count;
	}
	
	return count;
}

void filer_MenuEnter(void)
{
	SceIoDirent *entry;
	
	entry = filer_sorted_list[filer_data_pos + filer_cursor_y];
	if (entry->d_stat.st_mode & FIO_S_IFDIR)
	{
		if (strcmp(entry->d_name,".") != 0)
		{
			if (strcmp(entry->d_name,"..") == 0)
			{
				filer_MenuDirUp();
			}
			else
			{
				filer_DirectoryDown(filer_pwd,entry->d_name);
				filer_ReadDirectory();
				filer_cursor_y = 0;
			}
		}
	}
	else
	{
		strcpy(filer_filename,entry->d_name);
		status_BackTask(FILER_REL_SELECT);
	}
}

void filer_CheckUpdate(void)
{
	unsigned tick;
	
	tick = status_GetTick();
	if (tick - filer_CurrentTick > filer_NextFrame)
	{
		filer_CurrentTick = tick;
		status_RiseUpdateNotify();

		if (filer_cursor_cnt < 7)
			filer_cursor_cnt++;
		else
			filer_cursor_cnt = 0;
	}
}

void filer_KeyIn(int buttons)
{
	if (status_IsTaskBack())
	{
		filer_ResetButton(1);
		status_DisableUpdate(0);
		status_ClearTaskNotify();
		return;
	}
	
	filer_CheckUpdate();
	
	if (button_Check(&filer_btndata,buttons))
	{
		status_RiseUpdateNotify();

		if (buttons & PSP_CTRL_UP)
			filer_MenuUp();
			
		if (buttons & PSP_CTRL_DOWN)
			filer_MenuDown();

		if (buttons & PSP_CTRL_CIRCLE)
			filer_MenuEnter();

		if (buttons & PSP_CTRL_TRIANGLE)
			filer_MenuDirUp();
		
		if (buttons & PSP_CTRL_CROSS)
			status_BackTask(FILER_REL_CANCEL);
			
		if (buttons & PSP_CTRL_SELECT)
		{
			menu_Start();
			status_DisableUpdate(0);
			status_CallTask(TASK_MENU);
			return;
		}
	}
}

void filer_RemIn(int buttons)
{
	if (status_IsTaskBack())
	{
		filer_ResetButton(1);
		return;
	}
	
	if (button_Check(&filer_remdata,buttons))
	{
		if (!(buttons & PSP_HPRM_HOLD))
			status_RiseUpdateNotify();

		if (buttons & PSP_HPRM_BACK)
			filer_MenuUp();

		if (buttons & PSP_HPRM_FORWARD)
			filer_MenuDown();
		
		if (buttons & PSP_HPRM_PLAYPAUSE)
			filer_MenuEnter();
	}
}
