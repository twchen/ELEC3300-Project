#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
#define ADC_CH5  		5 		 	
	   									   
void Adc_Init(void); 				
u16  Get_Adc(u8 ch); 				
u16 Get_Adc_Average(u8 ch,u8 times);
short Get_Temperature(void);			
#endif 















