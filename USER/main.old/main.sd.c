#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "adc.h"
#include "beep.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h" 
#include "gui_draw.h"
#include "interface.h"
#include "sram.h"
#include "malloc.h"
#include "sdio_sdcard.h"

void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);
}


int main(void)
{ 
	u8 key;		 
	u32 sd_size;
	u8 t=0;	
	u8 *buf;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200); //initialize port transmision rate 115200
	
	LED_Init();
 	LCD_Init();
	KEY_Init();
	FSMC_SRAM_Init(); // init external SRAM
	my_mem_init(SRAMIN); // init internal memory, 128 KB
	my_mem_init(SRAMEX); // init external memory, 1024 KB
	my_mem_init(SRAMCCM); // init CCM memory, 64 KB, can only be used by CPU
	
	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"SD CARD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/15");   
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 0");	   
 	while(SD_Init())
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;
	}
	show_sdcard_info();	
 	POINT_COLOR=BLUE;	
										    
	LCD_ShowString(30,150,200,16,16,"SD Card OK    ");
	LCD_ShowString(30,170,200,16,16,"SD Card Size:     MB");
	LCD_ShowNum(30+13*8,170,SDCardInfo.CardCapacity>>20,5,16);
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			buf=mymalloc(0,512);
			if(SD_ReadDisk(buf,0,1)==0)
			{	
				LCD_ShowString(30,190,200,16,16,"USART1 Sending Data...");
				printf("SECTOR 0 DATA:\r\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);
				printf("\r\nDATA ENDED\r\n");
				LCD_ShowString(30,190,200,16,16,"USART1 Send Data Over!");
			}
			myfree(0,buf);
		}
		else if(key == KEY1_PRES)
		{
			printf("Key1 is pressed\n");
		}
		else if(key == KEY2_PRES)
		{
			printf("Key2 is pressed\n");
		}
		else if(key == WKUP_PRES)
		{
			printf("Wake up key is pressed\n");
		}
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;
			t=0;
		}
	}
	/*
	BEEP_Init();
	Adc_Init();
	tp_dev.init(); // initialize touchpad
	POINT_COLOR = BLACK;
	main_menu();
	return 0;
	*/
}
