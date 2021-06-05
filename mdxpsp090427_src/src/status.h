#ifndef __STATUS_H__
#define __STATUS_H__

#define STATUS_IDLE    0
#define STATUS_DISPLAY 1
#define STATUS_WAIT    2
#define STATUS_RESULT  3
#define STATUS_FINISH  4
#define STATUS_QUIT    100

void     status_GetStatusString(char *dest);
void     status_GetBatteryString(char *dest);
void     status_GetTimeString(char *dest);

int      status_GetBatteryPercent(void);

unsigned status_GetTick(void);
unsigned status_GetTickPerSec(void);

typedef struct 
{
	int task_id;
	int status;
	int disable_update;
	int result_int;
} t_taskmgr;

#define TASK_MAX   16

#define TASK_FILER  0
#define TASK_POPUP  1
#define TASK_VIEWER 2
#define TASK_PLAYER 3
#define TASK_MENU   4

#define TASK_QUIT   15

void  status_InitTask(void);
int   status_CallTask(int task_id);
int   status_BackTask(int result);

int  status_CurrentStackPos(void);
void status_DisableUpdate(int state);

int  status_GetDisableUpdate(int pos);
int  status_GetTaskID(int pos);

int  status_IsUpdateNeeded(void);

void status_RiseUpdateNotify(void);
void status_ClearUpdateNotify(void);

int  status_IsTaskBack(void);
int  status_GetResult(void);
void status_ClearTaskNotify(void);

int  status_GetPreviousTask(void);
int  status_GetCurrentTask(void);




#endif
