#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
#include "exfuns.h"  
#include "text.h"
#include "string.h"
#include "lwip_comm.h"

__audiodev audiodev;	  

void audio_start(void)
{
	audiodev.status=3<<0;
	I2S_Play_Start();
}

void audio_stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}

u16 audio_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
	DIR tdir;
	FILINFO tfileinfo;
	u8 *fn; 			 			   			     
	res=f_opendir(&tdir,(const TCHAR*)path);
	tfileinfo.lfsize=_MAX_LFN*2+1;
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)
		{
			res=f_readdir(&tdir,&tfileinfo);       		
			if(res!=FR_OK||tfileinfo.fname[0]==0)break;	
			fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)
			{
				rval++;
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
}



void audio_index_show(u16 index,u16 total)
{
	
	LCD_ShowxNum(60+0,230,index,3,16,0X80);		
	LCD_ShowChar(60+24,230,'/',16,0);
	LCD_ShowxNum(60+32,230,total,3,16,0X80); 	
}

void audio_msg_show(u32 totsec,u32 cursec,u32 bitrate)
{	
	static u16 playtime=0XFFFF;
	if(playtime!=cursec)					
	{
		playtime=cursec;
		
		LCD_ShowxNum(60,550,playtime/60,2,16,0X80);		
		LCD_ShowChar(60+16,550,':',16,0);
		LCD_ShowxNum(60+24,550,playtime%60,2,16,0X80);	
		LCD_ShowChar(60+40,550,'/',16,0); 	    	 
		
		LCD_ShowxNum(60+48,550,totsec/60,2,16,0X80);	
		LCD_ShowChar(60+64,550,':',16,0);
		LCD_ShowxNum(60+72,550,totsec%60,2,16,0X80);	
		
		LCD_ShowxNum(60+110,550,bitrate/1000,4,16,0X80);
		LCD_ShowString(60+110+32,550,200,16,16,"Kbps");	 
	} 		 
}

int audio_play(void)
{
	u8 res;
	DIR wavdir;	 		
	FILINFO wavfileinfo;
	u8 *fn;   			
	u8 *pname;			
	u16 totwavnum; 		
	u16 curindex;		
	u8 key;				
	u16 temp;
	u16 *wavindextbl;	
	
	WM8978_ADDA_Cfg(1,0);	
	WM8978_Input_Cfg(0,0,0);
	WM8978_Output_Cfg(1,0);	
	
	while(f_opendir(&wavdir,"0:/music"))
	{	    
		lwip_periodic_handle();
		Show_Str(60,190,240,16,"Error opening music dir",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);
		return 1;			  
	} 									  
	totwavnum=audio_get_tnum("0:/music"); 
	while(totwavnum==NULL)
	{
		lwip_periodic_handle();
		Show_Str(60,190,240,16,"No images files",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);
		return 1;			  
	}										   
	wavfileinfo.lfsize=_MAX_LFN*2+1;						
	wavfileinfo.lfname=mymalloc(SRAMIN,wavfileinfo.lfsize);	
	pname=mymalloc(SRAMIN,wavfileinfo.lfsize);				
	wavindextbl=mymalloc(SRAMIN,2*totwavnum);				
	while(wavfileinfo.lfname==NULL||pname==NULL||wavindextbl==NULL)
	{	    
		lwip_periodic_handle();
		Show_Str(60,190,240,16,"Memory allocation error",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);
		return 1;	  
	}  	 
	
	res=f_opendir(&wavdir,"0:/music"); 
	if(res==FR_OK)
	{
		curindex=0;
		while(1)
		{
			lwip_periodic_handle();
			temp=wavdir.index;								
			res=f_readdir(&wavdir,&wavfileinfo);       		
			if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	
			fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)
			{
				wavindextbl[curindex]=temp;
				curindex++;
			}	
		} 
	}   
	curindex=0;											
	res=f_opendir(&wavdir,(const TCHAR*)"0:/music"); 	
	while(res==FR_OK)
	{	
		lwip_periodic_handle();
		dir_sdi(&wavdir,wavindextbl[curindex]);			
		res=f_readdir(&wavdir,&wavfileinfo);       		
		if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	
		fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
		strcpy((char*)pname,"0:/music/");				
		strcat((char*)pname,(const char*)fn);  			
		LCD_Fill(100,130,240,200,WHITE);				
		Show_Str(100,130,240,16,fn,16,0);				
		//audio_index_show(curindex+1,totwavnum);
		key=audio_play_song(pname); 			 		
		if(key==KEY2_PRES)		
		{
			if(curindex)curindex--;
			else curindex=totwavnum-1;
		}else if(key==KEY0_PRES)
		{
			curindex++;		   	
			if(curindex>=totwavnum)curindex=0;
		}else break;	
	} 											  
	myfree(SRAMIN,wavfileinfo.lfname);	
	myfree(SRAMIN,pname);				
	myfree(SRAMIN,wavindextbl);		
	return 0;	
} 

u8 audio_play_song(u8* fname)
{
	u8 res;  
	res=f_typetell(fname); 
	switch(res)
	{
		case T_WAV:
		res=wav_play_song(fname);
		break;
		default:
		printf("can't play:%s\r\n",fname);
		res=KEY0_PRES;
		break;
	}
	return res;
}



























