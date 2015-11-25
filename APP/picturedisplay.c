#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "sdio_sdcard.h"    
#include "malloc.h"
#include "piclib.h"
#include "string.h"	
#include "text.h"
#include "math.h"	
#include "usart.h"
#include "ff.h" 
u16 pic_get_tnum(u8 *path)
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
			if((res&0XF0)==0X50) // the file is a picture
			{
				rval++;
			}	    
		}  
	} 
	return rval;
}

// 1 error;
// 0 ok
int picture_display(void)
{        
	u8 res;
	DIR picdir;
	FILINFO picfileinfo;
	u8 *fn;
	u8 *pname;
	u16 totpicnum; 
	u16 curindex;
	u8 key;
	u8 pause=0;
	u8 t;
	u16 temp;
	u16 *picindextbl;
	while(f_opendir(&picdir,"0:/pictures"))
	{
		Show_Str(30,170,240,16,"error opening PICTURE folder!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);    
		return 1;
	}  
	totpicnum=pic_get_tnum("0:/pictures");
	while(totpicnum==NULL)
	{	    
		Show_Str(30,170,240,16,"No pictures!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);    
		return 1;	  
	}
	picfileinfo.lfsize=_MAX_LFN*2+1;
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);
	pname=mymalloc(SRAMIN,picfileinfo.lfsize);
	picindextbl=mymalloc(SRAMIN,2*totpicnum);
	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)
	{	    
		Show_Str(30,170,240,16,"Memory allocation failed",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);  
		return 1;	  
	}  	
	res=f_opendir(&picdir,"0:/pictures");
	if(res==FR_OK)
	{
		curindex=0;
		while(1)
		{
			temp=picdir.index;							
			res=f_readdir(&picdir,&picfileinfo);
			if(res!=FR_OK||picfileinfo.fname[0]==0)break;
			fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)
			{
				picindextbl[curindex]=temp;
				curindex++;
			}	    
		} 
	}
	piclib_init();
	curindex=0;
	res=f_opendir(&picdir,(const TCHAR*)"0:/pictures");
	while(res==FR_OK)
	{	
		dir_sdi(&picdir,picindextbl[curindex]);
		res=f_readdir(&picdir,&picfileinfo);
		if(res!=FR_OK||picfileinfo.fname[0]==0)break;	
		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
		strcpy((char*)pname,"0:/pictures/");
		printf("width:%d, height:%d\n", lcddev.width,lcddev.height);
		strcat((char*)pname,(const char*)fn); 
		LCD_Clear(WHITE);
		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1); 
		Show_Str(2,2,240,16,pname,16,1);
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);
			if(t>250)key=KEY0_PRES; // time out
			if(key==KEY2_PRES) // prev
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES) // next
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;
				break;
			}else if(key==KEY1_PRES) // pausse
			{
				pause=!pause;
			}
			else if(key==WKUP_PRES)
			{
				goto END;
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	}
	END:	
	myfree(SRAMIN,picfileinfo.lfname);   
	myfree(SRAMIN,pname);   
	myfree(SRAMIN,picindextbl);
	return 0;
}











