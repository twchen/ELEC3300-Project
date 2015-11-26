#include "wavplay.h" 
#include "audioplay.h"
#include "usart.h" 
#include "delay.h" 
#include "malloc.h"
#include "ff.h"
#include "i2s.h"
#include "wm8978.h"
#include "key.h"
#include "led.h"
#include "touch.h"
#include "piclib.h"
#include "interface.h"
#include "lwip_comm.h"

__wavctrl wavctrl;		
vu8 wavtransferend=0;	
vu8 wavwitchbuf=0;		


void wav_init(void)
{
	WM8978_Init();				
	WM8978_HPvol_Set(40,40);	
	WM8978_SPKvol_Set(50);		
	WM8978_ADDA_Cfg(1, 0);
	WM8978_Input_Cfg(0, 0, 0);
	WM8978_Output_Cfg(1, 0);
}

u8 wav_decode_init(u8* fname,__wavctrl* wavx)
{
	FIL*ftemp;
	u8 *buf; 
	u32 br=0;
	u8 res=0;
	
	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;
	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	buf=mymalloc(SRAMIN,512);
	if(ftemp&&buf)	
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	
			riff=(ChunkRIFF *)buf;		
			if(riff->Format==0X45564157)
			{
				fmt=(ChunkFMT *)(buf+12);	
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);
				if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;
				else wavx->datastart=12+8+fmt->ChunkSize;  
				data=(ChunkDATA *)(buf+wavx->datastart);	
				if(data->ChunkID==0X61746164)
				{
					wavx->audioformat=fmt->AudioFormat;		
					wavx->nchannels=fmt->NumOfChannels;		
					wavx->samplerate=fmt->SampleRate;		
					wavx->bitrate=fmt->ByteRate*8;			
					wavx->blockalign=fmt->BlockAlign;		
					wavx->bps=fmt->BitsPerSample;			
					
					wavx->datasize=data->ChunkSize;			
					wavx->datastart=wavx->datastart+8;		

					printf("wavx->audioformat:%d\r\n",wavx->audioformat);
					printf("wavx->nchannels:%d\r\n",wavx->nchannels);
					printf("wavx->samplerate:%d\r\n",wavx->samplerate);
					printf("wavx->bitrate:%d\r\n",wavx->bitrate);
					printf("wavx->blockalign:%d\r\n",wavx->blockalign);
					printf("wavx->bps:%d\r\n",wavx->bps);
					printf("wavx->datasize:%d\r\n",wavx->datasize);
					printf("wavx->datastart:%d\r\n",wavx->datastart);  
				}else res=3;
			}else res=2;
			
		}else res=1;
	}
	f_close(ftemp);
	myfree(SRAMIN,ftemp);
	myfree(SRAMIN,buf); 
	return 0;
}






u32 wav_buffill(u8 *buf,u16 size,u8 bits)
{
	u16 readlen=0;
	u32 bread;
	u16 i;
	u8 *p;
	if(bits==24)
	{
		readlen=(size/4)*3;							
		f_read(audiodev.file,audiodev.tbuf,readlen,(UINT*)&bread);	
		p=audiodev.tbuf;
		for(i=0;i<size;)
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		} 
		bread=(bread*4)/3;		
	}else 
	{
		f_read(audiodev.file,buf,size,(UINT*)&bread);
		if(bread<size)
		{
			for(i=bread;i<size-bread;i++)buf[i]=0; 
		}
}
return bread;
}  

void wav_i2s_dma_tx_callback(void) 
{
	u16 i;
	if(DMA1_Stream4->CR&(1<<19)) 
	{
		wavwitchbuf=0;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)
			{
				audiodev.i2sbuf1[i]=0;
			}
		}
	}else 
	{
		wavwitchbuf=1;
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)
			{
				audiodev.i2sbuf2[i]=0;
			}
		}
	}
	wavtransferend=1;
} 



void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;  	
	wavx->totsec=wavx->datasize/(wavx->bitrate/8);	
	fpos=fx->fptr-wavx->datastart; 					
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;	
}






u8 wav_play_song(u8* fname)
{
	u16 prev_sta = 0x0;
	u8 key;
	u8 t=0; 
	u8 res;  
	u32 fillnum; 
	audiodev.file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	audiodev.i2sbuf1=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audiodev.i2sbuf2=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audiodev.tbuf=mymalloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	if(audiodev.file&&audiodev.i2sbuf1&&audiodev.i2sbuf2&&audiodev.tbuf)
	{ 
		res=wav_decode_init(fname,&wavctrl);
		if(res==0)
		{
			if(wavctrl.bps==16)
			{
				WM8978_I2S_Cfg(2,0);	
				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);		
			}else if(wavctrl.bps==24)
			{
				WM8978_I2S_Cfg(2,2);	
				I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		
			}
			I2S2_SampleRate_Set(wavctrl.samplerate);
			I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE/2); 
			i2s_tx_callback=wav_i2s_dma_tx_callback;			
			audio_stop();
			res=f_open(audiodev.file,(TCHAR*)fname,FA_READ);	
			if(res==0)
			{
				f_lseek(audiodev.file, wavctrl.datastart);		
				fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				audio_start();  
				while(res==0)
				{
					while(wavtransferend==0){
						lwip_periodic_handle();
					}
					wavtransferend=0;
					if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)
					{
						res=KEY0_PRES;
						break;
					}
					if(wavwitchbuf)fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
					else fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
					while(1)
					{
						lwip_periodic_handle();
						key=KEY_Scan(0); 
						if(key==KEY1_PRES)
						{
							if(audiodev.status&0X01){
								audiodev.status&=~(1<<0);
								ai_load_picfile("0:/systems/play.jpg", 200, 640, 60, 61, 1);
							}
							else{
								audiodev.status|=0X01;  
								ai_load_picfile("0:/systems/pause.jpg", 200, 638, 58, 58, 1);
							}
						}
						if(key==KEY2_PRES||key==KEY0_PRES||key==WKUP_PRES)
						{
							res=key;
							break; 
						}
						if(is_web_music_player == 1 && music_on == 0){
							res=WKUP_PRES;
							break;
						}
						wav_get_curtime(audiodev.file,&wavctrl);
						audio_msg_show(wavctrl.totsec,wavctrl.cursec,wavctrl.bitrate);
						t++;
						if(t==20)
						{
							t=0;
 							LED0=!LED0;
						}
						if((audiodev.status&0X01)==0)delay_ms(10);
						else break;
					}
					/*
					while(1){
						tp_dev.scan(0);
						key=KEY_Scan(0);
						if(((tp_dev.sta) & 0x80 && !(prev_sta & 0x80))|| key != 0){
							if(tp_dev.y[4] < 620){}
							else if(tp_dev.y[4] < 700){
								if(tp_dev.x[4] < 90){}
								else if(tp_dev.x[4] < 190 || key == KEY2_PRES){
									// prev
									res=KEY2_PRES;
									delay_ms(100);
									break; 
								}
								else if(tp_dev.x[4] < 270 || key == KEY1_PRES){
									if(audiodev.status&0X01){
										// play
										ai_load_picfile("0:/systems/play.jpg", 200, 640, 60, 61, 1);
										audiodev.status&=~(1<<0);
									}
									else{
										//stop
										ai_load_picfile("0:/systems/pause.jpg", 200, 638, 58, 58, 1);
										audiodev.status|=0X01;
									}
									delay_ms(100);
								}
								else if(tp_dev.x[4] < 380 || key == KEY0_PRES){
									// next
									res=KEY0_PRES;
									delay_ms(100);
									break;
								}
							}
							else if((tp_dev.x[4] > 190 && tp_dev.x[4] < 270) || key == WKUP_PRES){
								// return
								res = WKUP_PRES;
								delay_ms(100);
								break;
							}
							wav_get_curtime(audiodev.file,&wavctrl);
							audio_msg_show(wavctrl.totsec,wavctrl.cursec,wavctrl.bitrate);

						}
						prev_sta = tp_dev.sta;
						tp_dev.sta &= 0x7f;
						if((audiodev.status&0X01)==0)delay_ms(10);
						else break;
					}
					*/
					lwip_periodic_handle();
				}
				audio_stop(); 
			}else res=0XFF; 
		}else res=0XFF;
	}else res=0XFF;
	myfree(SRAMIN,audiodev.tbuf);	
	myfree(SRAMIN,audiodev.i2sbuf1);
	myfree(SRAMIN,audiodev.i2sbuf2);
	myfree(SRAMIN,audiodev.file);	
	return res;
}









