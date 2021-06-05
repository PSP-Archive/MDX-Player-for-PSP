#include <stdio.h>
#include <string.h>

#include "config.h"

typedef struct
{
	char pcmdir[2048];
	char musdir[2048];
	char musfile[256];
	int repeat;
} t_config;

static t_config config_data;

static char config_file[2048 + 256];

#define CONFIG_TYPE_STR 0
#define CONFIG_TYPE_INT 1

typedef struct
{
	int  id;
	char *label;
	int  type;
	void *data;
} t_conftext;

static t_conftext config_labels[] =
{
	{CONFIG_ID_PCMDIR,"pcm_directory" ,
	 CONFIG_TYPE_STR , config_data.pcmdir },
	{CONFIG_ID_MUSDIR,"mus_directory" , 
	 CONFIG_TYPE_STR , config_data.musdir },
	{CONFIG_ID_MUSFILE,"mus_file" , 
	 CONFIG_TYPE_STR , config_data.musfile },
	{CONFIG_ID_REPEAT,"repeat" , 
	 CONFIG_TYPE_INT , &config_data.repeat },

	{0,NULL,0,NULL}
};

void config_cutlf(char *buf)
{
	int i;
	i = strlen(buf);
	
	if (!i)
		return;
	else
		i--;
	
	while(i >= 0 && buf[i] < 0x20) 
		buf[i--] = 0;
}

void config_Init(void)
{
	config_file[0] = 0;
	config_data.pcmdir[0] = 0;
	config_data.musdir[0] = 0;
}

void config_ReadFile(void)
{
	FILE *fp;
	int  i;
	char buf[256];

	
	fp = fopen(config_file,"r");
	
	if (!fp)
		return;
	
	do
	{
		buf[0] = 0;
		fgets(buf,256,fp);
		config_cutlf(buf);
		
		if (buf[0] == '*')
		{
			for(i=0; config_labels[i].label; i++)
			{
				if (strcmp(buf+1,config_labels[i].label) == 0)
				{
					fgets(buf,256,fp);
					config_cutlf(buf);
					switch(config_labels[i].type)
					{
						case CONFIG_TYPE_STR:
							strcpy(config_labels[i].data,buf);
						break;
						case CONFIG_TYPE_INT:
							sscanf(buf,"%d",(int *)config_labels[i].data);
						break;
					}
					break;
				}
			}
		}
		
	}while(!feof(fp));

	fclose(fp);
}

void config_WriteFile(void)
{
	FILE *fp;
	int  i;

	
	fp = fopen(config_file,"w");
	
	if (!fp)
		return;
	
	fprintf(fp,"# config file\n");
	
	for(i=0; config_labels[i].label; i++)
	{
		switch (config_labels[i].type)
		{
			case CONFIG_TYPE_STR:
				fprintf(fp,"*%s\n%s\n\n",
					config_labels[i].label,
					(char *)config_labels[i].data);
				break;
			case CONFIG_TYPE_INT:
				fprintf(fp,"*%s\n%d\n\n",
					config_labels[i].label,
					*(int *)config_labels[i].data);
				break;
		}
	}
	fclose(fp);
}

int  config_GetInt(int config_id)
{
	int i;
	for(i=0; config_labels[i].label; i++)
	{
		if ( config_labels[i].id == config_id)
			return *(int *)config_labels[i].data;
	}
	return 0;
}

void config_GetStr(int config_id,char *str)
{
	int i;
	for(i=0; config_labels[i].label; i++)
	{
		if ( config_labels[i].id == config_id)
		{
			strcpy(str,config_labels[i].data);
			break;
		}
	}
	return;
}

void config_SetInt(int config_id,int value)
{
	int i;
	for(i=0; config_labels[i].label; i++)
	{
		if ( config_labels[i].id == config_id)
		{
			*(int *)config_labels[i].data = value;
			break;
		}
	}
	return;
}

void config_SetStr(int config_id,const char *str)
{
	int i;
	for(i=0; config_labels[i].label; i++)
	{
		if ( config_labels[i].id == config_id)
		{
			strcpy(config_labels[i].data,str);
			break;
		}
	}
	return;
}

void config_CutLastItem(char *dest,const char *path)
{
	char *p;
	
	strcpy(dest,path);
	
	p = strrchr(dest,'/');

	if (p)
		*p = 0;

	return;
}

void config_SetConfigFilePath(const char *path)
{
	strcpy(config_file,path);
}

void config_GetConfigFilePath(char *path)
{
	strcpy(path,config_file);
}


void config_SetConfigFileName(const char *file)
{
	char *p;
	
	p = strrchr(config_file,'/');
	if (p)
		strcpy(p+1,file);
	else
		strcpy(config_file,file);
}

#if 0

int main(int argc,char *argv[])
{
	char buf[256];

	config_Init();
	
	config_SetConfigFileName("config.cfg");
	
	config_CutLastItem(buf,argv[0]);
	
	config_SetInt(CONFIG_ID_REPEAT,1234);
	config_SetStr(CONFIG_ID_MUSDIR,buf);

	config_ReadFile();
	config_GetStr(CONFIG_ID_MUSDIR,buf);
	
	printf("MUSDIR = %s\n",buf);

	printf("REPEAT = %d\n",config_GetInt(CONFIG_ID_REPEAT));

	
	config_WriteFile();

	return 0;
}

#endif
