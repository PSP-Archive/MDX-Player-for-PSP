#ifndef __FILER_H__
#define __FILER_H__

#define FILER_REL_CANCEL 0
#define FILER_REL_SELECT 1

void filer_Init(const char *path);
int  filer_Display(void);
void filer_KeyIn(int buttons);
void filer_RemIn(int buttons);


void filer_SetPath(const char *pwd,const char *file);
void filer_GetPath(char *pwd,char *file);
void filer_ReloadDir(void);


void filer_SetMessage(char *msg);

int filer_SearchNextFile(int forward,int count);

int filer_GetStatus(void);

#endif
