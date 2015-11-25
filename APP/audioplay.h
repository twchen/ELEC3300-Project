#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H
#include "sys.h"
#include "ff.h"
#include "wavplay.h"

#define I2S_TX_DMA_BUF_SIZE    8192	


typedef __packed struct
{  
	// 2 i2s buffer
	u8 *i2sbuf1;
	u8 *i2sbuf2; 
	u8 *tbuf;
	FIL *file;
	
	u8 status; // B11: pause, B11: play
				//B0X: stop
}__audiodev; 
extern __audiodev audiodev;


void wav_i2s_dma_callback(void);

void audio_start(void);
void audio_stop(void);
u16 audio_get_tnum(u8 *path);
void audio_index_show(u16 index,u16 total);
void audio_msg_show(u32 totsec,u32 cursec,u32 bitrate);
int audio_play(void);
u8 audio_play_song(u8* fname);


#endif












