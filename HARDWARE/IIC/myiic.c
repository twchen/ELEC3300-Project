#include "myiic.h"
#include "delay.h"
#include "usart.h"

void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; // output push pull
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // push up
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDA=1;
}

void IIC_Start(void)
{
	SDA_OUT(); // output mode
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0; // start to send or receive data
}

void IIC_Stop(void)
{
	SDA_OUT();  // output mode
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;
	delay_us(4);							   	
}

// return 1: fail
//				0: success
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN(); // SDA input mode
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;
	return 0;  
}

void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_OUT(); 	    
	IIC_SCL=0;
	for(t=0;t<8;t++)
	{              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}	 
}

u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}

void LED_SendOneByte(u8 data)
{
	IIC_Start();
	IIC_Send_Byte(LED_DEVICE_ADDRESS);
	IIC_Wait_Ack();
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	IIC_Stop();
	//delay_ms(10);
}

void LED_SendBytes(u8 *data, u8 num)
{
	IIC_Start();
	IIC_Send_Byte(LED_DEVICE_ADDRESS);
	//delay_ms(1);
//	if(IIC_Wait_Ack()){
//		printf("failied\n");
//	}
	IIC_Send_Byte(LED_START_OF_DATA);
//	//delay_ms(1);
//	if(IIC_Wait_Ack()){
//		printf("failied\n");
//	}
	while(num--)
	{
		IIC_Send_Byte(*data);
		data++;
		//delay_ms(1);
//		if(IIC_Wait_Ack()){
//			printf("failied\n");
	}
//	}
	IIC_Send_Byte(LED_END_OF_DATA);
//	//delay_ms(1);
//	if(IIC_Wait_Ack()){
//		printf("failied\n");
//	}
	IIC_Stop();
}























