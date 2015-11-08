#include "lcd.h"  
#include "touch.h" 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"

//�����Ļ�������Ͻ���ʾ"RST"
void Load_Drow_Dialog(void);

////////////////////////////////////////////////////////////////////////////////
//���ݴ�����ר�в���
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);


//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);

//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2);

//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);

////////////////////////////////////////////////////////////////////////////////
 //5�����ص����ɫ(���ݴ�������)												  
//���败�������Ժ���
void rtp_test(void);

//���ݴ��������Ժ���
void ctp_test(void);
