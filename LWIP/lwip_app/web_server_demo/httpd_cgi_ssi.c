#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "lwip_comm.h"
#include "led.h"
#include "beep.h"
#include "adc.h"
#include "interface.h"
#include "lcd.h"

#include <string.h>
#include <stdlib.h>

#define NUM_CONFIG_CGI_URIS	1
#define NUM_CONFIG_SSI_TAGS	5

u8 music_on = 0;
extern void set_handler(void (*handler)(u16, u16));
extern short Get_Temperature(void);

const char* Switch_CGI_Handler(int iIndex,int iNumParams,char *pcParam[],char *pcValue[]);

static const char *ppcTAGs[]= // tag of ssi
{
	"t", // temperature
	"h", // humidity,
	"lamp1",
	"lamp2",
	"alarm"
};

static const tCGI ppcURLs[]= // cgi programs
{
	{"/switch.cgi",Switch_CGI_Handler},
};

static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams)
{
	int iLoop;
	for(iLoop = 0;iLoop < iNumParams;iLoop ++ )
	{
		if(strcmp(pcToFind,pcParam[iLoop]) == 0)
		{
			return (iLoop);
		}
	}
	return (-1);
}

void Temperate_Handler(char *pcInsert)
{
	char Digit1=0, Digit2=0, Digit3=0, Digit4=0;
	short Temperate = 0;
		
	Temperate = Get_Temperature();
	Digit1 = Temperate / 10000;
	Digit2 = (Temperate % 10000)/1000;
    Digit3 = (Temperate % 1000)/100 ;
    Digit4 = (Temperate % 100)/10;

	*pcInsert 		= (char)(Digit1+0x30);
	*(pcInsert+1) = (char)(Digit2+0x30);
	*(pcInsert+2)	=	(char)(Digit3+0x30);
	*(pcInsert+3) = '.';
	*(pcInsert+4) = (char)(Digit4+0x30);
}

void Device_Handler(char *pcInsert, u8 status)
{
	if(status){
		*pcInsert 		= 'O';
		*(pcInsert+1) = 'n';
	}
	else{
		*pcInsert 		= 'O';
		*(pcInsert+1) = 'f';
		*(pcInsert+2)	=	'f';
	}
}

static u16_t SSIHandler(int iIndex,char *pcInsert,int iInsertLen)
{
	switch(iIndex)
	{
		case 0:
			Temperate_Handler(pcInsert);
			break;
		case 1:
			//Humidity_Handler(pcInsert);
			break;
		case 2:
			Device_Handler(pcInsert, LAMP1);
			break;
		case 3:
			Device_Handler(pcInsert, LAMP2);
			break;
		case 4:
			Device_Handler(pcInsert, BEEP);
			break;
	}
	return strlen(pcInsert);
}

const char* Switch_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	u8 i=0;
	iIndex = FindCGIParameter("device",pcParam,iNumParams);
	if (iIndex != -1)
	{
		for (i=0; i<iNumParams; i++)
		{
		  if (strcmp(pcParam[i] , "device")==0)
		  {
				if(strcmp(pcValue[i], "lamp1") ==0)
					LED0 = !LED0;
				else if(strcmp(pcValue[i],"lamp2") == 0)
					LED1 = !LED1;
				else if(strcmp(pcValue[i], "alarm") == 0)
					BEEP = !BEEP;
				else if(strcmp(pcValue[i], "music") == 0){
					if(music_on){
						music_on = 0;
					}
					else{
						music_on = 1;
						set_handler(music_handler);
					}
				}
			}
		}
	 }
	return "/result.html";		
}


//SSI handler init
void httpd_ssi_init(void)
{  
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

// CGI handler init
void httpd_cgi_init(void)
{ 
  http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}


