#include <stdio.h>
#include <SDL.h>


#include "mdxmini.h"

int audio_on = 0;

#define PCM_BLOCK 512
#define PCM_BYTE_PER_SAMPLE 2
#define PCM_CH  2


#define BUF_BLKSIZE PCM_BLOCK


#define BUF_BLKNUM 16

#define BUF_LEN ( BUF_BLKSIZE * PCM_CH * BUF_BLKNUM )

int buf_wpos = 0;
int buf_ppos = 0;

short buffer[ BUF_LEN ];

void audio_callback(void *param,Uint8 *data,int len)
{
	int i,j;
	
	short *pcm = (short *)data;

	if (!audio_on)
	{
		memset(data,0,len);
		return;
	}
	
	for( i = 0; i < len/2; i++ )
	{
		pcm[i] = buffer[buf_ppos++];
		if (buf_ppos >= BUF_LEN)
				buf_ppos = 0;
	}
}

int init_audio(void)
{
	SDL_AudioSpec af;

	if (SDL_Init(SDL_INIT_AUDIO))
	{
		printf("Failed to Initialize!!\n");
		return 1;
	}
	
	af.freq = 44100;
	af.format = AUDIO_S16;
	af.channels = 2;
	af.samples = 512;
	af.callback = audio_callback;
	af.userdata = NULL;

	if (SDL_OpenAudio(&af,NULL) < 0)
	{
		printf("Audio Error!!\n");
		return 1;
	}
	
	memset(buffer,0,sizeof(buffer));
	
	SDL_PauseAudio(0);
	return 0;
}

void free_audio(void)
{
	SDL_CloseAudio();
}

void player_loop(t_mdxmini *data)
{

	int i,j,len;
	
	int total;
	int sec,old_sec;
	int sec_sample;
	int freq;
	int next;
	
	len = mdx_get_length(data);
	
	
	
	freq = 44100;
	old_sec = total = sec = sec_sample = 0;
	
	next = 1;
	
	do
	{
		// waiting for next block
		while(buf_wpos >= buf_ppos &&
			  buf_wpos <  buf_ppos + ( PCM_BLOCK * PCM_CH ) )
		{
			SDL_Delay(10);
		}
		
		next = mdx_calc_sample( data , buffer + buf_wpos, PCM_BLOCK );
		
		buf_wpos += PCM_BLOCK * PCM_CH;
		if (buf_wpos >= BUF_LEN)
				buf_wpos = 0;
		
		total += PCM_BLOCK;
		sec_sample += PCM_BLOCK;
		while ( sec_sample >= freq )
		{
			sec_sample -= freq;
			sec++;
		}
		if (sec != old_sec)
		{
			old_sec = sec;
			printf("Time : %02d:%02d / %02d:%02d\r\r",
				sec / 60 , sec % 60 , len / 60,len % 60);
			fflush(stdout);
				
		}

	}while(sec < len);

}



int main ( int argc, char *argv[] )
{
	FILE *fp;
	
	t_mdxmini mini;
	
	
	printf("MDXPLAY on SDL Version 090426\n");
	
	if (argc < 2)
	{
		printf("Usage mdxplay [file]\n");
		return 1;
	}
	
	if (init_audio())
	{
		return 0;
	}
	
	audio_on = 1;
		
	if (mdx_open(&mini,argv[1]))
	{
		printf("File open error\n");
		return 0;
	}
	
	player_loop(&mini);
	
	
	mdx_stop(&mini);

	free_audio();
	

	return 0;
}