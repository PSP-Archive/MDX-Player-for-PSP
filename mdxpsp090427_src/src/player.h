#ifndef __PLAYER_H__
#define __PLAYER_H__

#define PLAYER_REL_STOP 0
#define PLAYER_REL_NEXT 1
#define PLAYER_REL_PREV 2

void player_Init(char *execPath);
int  player_Display(void);

void player_KeyIn(int buttons);
void player_RemIn(int buttons);

void player_StartFiler(void);
int  player_PlayFile(const char *dir,const char *file);
void player_Stop(void);
void player_CheckUpdate(void);

void player_SetSongLen(int len);
void player_SetSongPos(int pos);

void player_SetPCMdir(char *pcmdir);
void player_SetMUSdir(char *musdir);

#endif
