#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	 
#include "wm8978.h"	 
#include "audioplay.h"
#include "recorder.h"
 
int main(void)
{        
 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);  
	uart_init(115200);		
	LED_Init();					
	usmart_dev.init(84);		
 	LCD_Init();					
 	KEY_Init();					
	W25QXX_Init();				
	WM8978_Init();				
	WM8978_HPvol_Set(40,40);	
	WM8978_SPKvol_Set(50);		
	
	my_mem_init(SRAMIN);		
	my_mem_init(SRAMCCM);		
	exfuns_init();				
  	f_mount(fs[0],"0:",1); 		
	POINT_COLOR=RED;      
	while(font_init()) 			
	{	    
		LCD_ShowString(30,40,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,40,240,66,WHITE);
		delay_ms(200);				  
	}  	 
	POINT_COLOR=RED;      
 	Show_Str(30,40,200,16,"Explorer STM32开发板",16,0);				    	 
	Show_Str(30,60,200,16,"录音机实验",16,0);				    	 
	Show_Str(30,80,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,100,200,16,"2014年6月6日",16,0); 
	while(1)
	{ 
		wav_recorder();
		
	} 
}




