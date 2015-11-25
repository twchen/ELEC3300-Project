#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h" 
#include "adc.h"
#include "beep.h"
#include "touch.h" 
#include "gui_draw.h"
#include "interface.h"
#include "sram.h"
#include "malloc.h"
#include "sdio_sdcard.h"
#include "exfuns.h"
#include "fontupd.h"
#include "w25qxx.h"
#include "24cxx.h"
#include "ff.h"
#include "text.h"
#include "wm8978.h"
#include "audioplay.h"
#include "LAN8720.h"
#include "lwip_comm.h"
#include "lwip_timer.h"
#include "lwip/netif.h"
#include "lwipopts.h"
#include "httpd.h"
#include "piclib.h"

void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==1)
	{
		sprintf((char*)buf,"MAC    :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
		LCD_ShowString(30,190,210,16,16,buf); 
	}
	else 
	{
		sprintf((char*)buf,"MAC      :%d.%d.%d.%d.%d.%d",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
		LCD_ShowString(30,190,210,16,16,buf); 
	}	
}

int main(void)
{ 
	int counter = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200); //initialize port transmision rate 115200
	
	LED_Init();
 	LCD_Init();
	KEY_Init();
	BEEP_Init();
	FSMC_SRAM_Init(); // init external SRAM
	W25QXX_Init();
	wav_init();
	my_mem_init(SRAMIN); // init internal memory, 128 KB
	my_mem_init(SRAMEX); // init external memory, 1024 KB
	my_mem_init(SRAMCCM); // init CCM memory, 64 KB, can only be used by CPU
	exfuns_init();				//为fatfs相关变量申请内存  
  f_mount(fs[0],"0:",1); 		//挂载SD卡  
	Adc_Init();
	tp_dev.init(); // initialize touchpad
	TIM3_Int_Init(999, 839); // lwip timer init
	piclib_init(); 
	while(lwip_comm_init())
	{
		LCD_ShowString(30,110,200,20,16,"LWIP Init Falied!");
		delay_ms(1200);
		LCD_Fill(30,110,230,130,WHITE);
		LCD_ShowString(30,110,200,16,16,"Retrying...");  
	}
#if LWIP_DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))
	{
		lwip_periodic_handle();
	}
#endif
	show_address(lwipdev.dhcpstatus);
	delay_ms(5000);
	//httpd_init();
	POINT_COLOR = BLACK;
	draw_main_menu();
	set_handler(main_menu_handler);
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}
	u16 prev_sta = 0x0;
	while(1){
		tp_dev.scan(0);
		if((tp_dev.sta) & 0x80 && !(prev_sta & 0x80)){
			current_handler(tp_dev.x[4], tp_dev.y[4]);
		}
		prev_sta = tp_dev.sta;
		tp_dev.sta &= 0x7f;
		/*
		if(counter++ >= 100){
			LED0 = !LED0;
			counter = 0;
		}
		*/
		lwip_periodic_handle();
	}
}
