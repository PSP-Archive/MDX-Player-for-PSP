#ifndef __PCM_H__
#define __PCM_H__

#define PCM_BLKS 16
#define PCM_CH 2
#define PCM_BLOCKSIZE 1024

void pcm_init(void);
void pcm_start(void);
void pcm_stop(void);

int  pcm_chkEmpty(int len);
void pcm_setBuffer(void *buf,int len);

void pcm_clearBuffer(void);

void pcm_clearTime(void);
int  pcm_getTime(void);



#endif
