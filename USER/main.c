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

int main(void)
{ 

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
	
	BEEP_Init();
	Adc_Init();
	tp_dev.init(); // initialize touchpad
	POINT_COLOR = BLACK;
	main_menu();
	return 0;
}
