#include <stdio.h>
#include <psppower.h>
#include <psprtc.h>

#include "status.h"

void status_GetTimeString(char *dest)
{
	pspTime pt;
	
	sceRtcGetCurrentClockLocalTime(&pt);
	
	sprintf(dest,"%02d:%02d",pt.hour,pt.minutes);
}

void status_GetBatteryString(char *dest)
{
	int batt;
	
	batt = scePowerGetBatteryLifePercent();

	if (batt < 0)
		sprintf(dest,"[---%%]");
	else
		sprintf(dest,"[%3d%%]",batt);
}

int  status_GetBatteryPercent(void)
{
	return scePowerGetBatteryLifePercent();
}

void status_GetStatusString(char *dest)
{
	char buf[64];
	char buf2[64];
	
	status_GetTimeString(buf);
	status_GetBatteryString(buf2);
	sprintf(dest,"%s %s",buf,buf2);
}

unsigned status_GetTickPerSec(void)
{
	return sceRtcGetTickResolution();
}

unsigned status_GetTick(void)
{
	u64 tick;
	sceRtcGetCurrentTick(&tick);

	return tick;
}


static int status_TaskIsBack;
static int status_CurrentTask;
static int status_PreviousTask;
static int status_StackPos;



static t_taskmgr status_TaskList[TASK_MAX];

static int status_UpdateList[TASK_MAX];
static int status_IsUpdate;

void  status_InitTask(void)
{
	int i;

	t_taskmgr *task;
	
	status_TaskIsBack = 0;
	status_StackPos   = 0;
	status_IsUpdate   = 0;
	status_CurrentTask  = 0;
	status_PreviousTask = 0;
	
	for(i=0; i < TASK_MAX; i++)
	{
		status_UpdateList[i] = 0;
	
		task = &status_TaskList[i];
		task->task_id     = TASK_QUIT;
		task->status      = 0;
		task->disable_update      = 0;
		task->result_int  = 0;
	}
}

int  status_CurrentStackPos(void)
{
	return status_StackPos;
}

void status_DisableUpdate(int state)
{
	if (status_StackPos < 0)
		return;
	
	status_TaskList[status_StackPos].disable_update = state;
}

int status_GetDisableUpdate(int pos)
{
	if (pos < 0)
		return -1;
	
	return status_TaskList[pos].disable_update;
}
int status_GetTaskID(int pos)
{
	if (pos < 0)
		return -1;
	
	return status_TaskList[pos].task_id;
}

int   status_IsUpdateNeeded(void)
{
	return status_IsUpdate;
}

void  status_ClearUpdateNotify(void)
{
	status_IsUpdate = 0;
}

void status_RiseUpdateNotify(void)
{
	status_IsUpdate = 1;
}


int  status_CallTask(int task_id)
{
	t_taskmgr *task;

	status_StackPos ++;
	if (status_StackPos < TASK_MAX)
	{
		task = &status_TaskList[status_StackPos];
		task->task_id = task_id;
		task->disable_update = 0;
		task->status = 0;
		task->result_int = 0;

		status_PreviousTask = status_CurrentTask;
		status_CurrentTask = task_id;

		return 0;
	}
	else
	{
		status_StackPos--;
		return -1;
	}
}

int  status_BackTask(int result)
{
	t_taskmgr *task;
	
	if (status_StackPos > 0)
	{
		status_StackPos --;

		task = &status_TaskList[status_StackPos];
		task->result_int = result;
		
		status_PreviousTask = status_CurrentTask;
		status_CurrentTask = task->task_id;

		status_TaskIsBack = 1;
	}
	else
	{
		return -1;
	}
	return 0;
}

int  status_IsTaskBack(void)
{
	return status_TaskIsBack;
}

int  status_GetPreviousTask(void)
{
	return status_PreviousTask;
}

int  status_GetCurrentTask(void)
{
	return status_CurrentTask;
}

int  status_GetResult(void)
{
	return status_TaskList[status_StackPos].result_int;
}

void status_ClearTaskNotify(void)
{
	status_TaskIsBack = 0;
}
