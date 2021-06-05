#include <pspkernel.h>
#include <pspaudiolib.h>
#include <pspaudio.h>

#include <string.h>

#include "status.h"
#include "graph.h"
#include "font.h"
#include "button.h"

#include "pcm.h"

int pcm_on = 0;

int pcm_time = 0;
int pcm_datacnt = 0;

#define MAX_PCMBUF PCM_BLOCKSIZE * PCM_CH * PCM_BLKS

static short pcm_buf[MAX_PCMBUF];

static int pcm_playpos;
static int pcm_bufpos;

void pcm_callback(void* buf, unsigned int length, void *userdata)
{
	int i;

	short *samp = (short *)buf;
	
	if (!pcm_on)
	{
		for(i=0; i < length; i++)
		{
			samp[ i<<1 ]    = 0;
			samp[(i<<1)+1 ] = 0;
		}
	}
	else
	{
		for(i = 0; i < length; i++)
		{
			samp[ i<<1   ] = pcm_buf[pcm_playpos];
			samp[(i<<1)+1] = pcm_buf[pcm_playpos + 1];
			pcm_playpos += 2;
			if (pcm_playpos >= MAX_PCMBUF) 
				pcm_playpos = 0;
		}
		
		
		pcm_datacnt += length;
		if (pcm_datacnt >= 44100) 
		{
			pcm_datacnt -= 44100;
			pcm_time ++;
		}
	}
}

void pcm_clearTime(void)
{
	pcm_datacnt = 0;
	pcm_time    = 0;
}

int pcm_getTime(void)
{
	return pcm_time;
}

int pcm_chkEmpty(int len)
{
	if (pcm_playpos >= pcm_bufpos && 
		pcm_playpos < (pcm_bufpos + (len * 2)) )
				return 0;

	return 1;
}

void pcm_clearBuffer(void)
{
	memset(pcm_buf , 0 , MAX_PCMBUF * sizeof(short));
	
	pcm_bufpos  = 0;
	pcm_playpos = 0;
}

void pcm_setBuffer(void *buf,int len)
{
	memcpy(pcm_buf + pcm_bufpos, buf , len * 2 * 2);
	pcm_bufpos += (len * 2);
	
	if (pcm_bufpos >= MAX_PCMBUF)
		pcm_bufpos = 0;
}


void pcm_init(void)
{
	pcm_on = 0;
	
	pcm_clearBuffer();
	
	pcm_clearTime();
	
	pspAudioInit();
	pspAudioSetChannelCallback(0, pcm_callback, NULL);
}

void pcm_start(void)
{
	pcm_on = 1;
}

void pcm_stop(void)
{
	pcm_on = 0;
}

