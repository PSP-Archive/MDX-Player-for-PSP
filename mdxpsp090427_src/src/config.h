#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_ID_PCMDIR  0
#define CONFIG_ID_MUSDIR  1
#define CONFIG_ID_MUSFILE 2
#define CONFIG_ID_REPEAT  3


void config_Init(void);

void config_CutLastItem(char *dest,const char *path);
void config_SetConfigFilePath(const char *path);
void config_GetConfigFilePath(char *path);
void config_SetConfigFileName(const char *file);

int  config_GetInt(int config_id);
void config_GetStr(int config_id,char *str);

void config_SetInt(int config_id,int value);
void config_SetStr(int config_id,const char *str);



void config_WriteFile();
void config_ReadFile();


#endif


