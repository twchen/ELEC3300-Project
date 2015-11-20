#include "lcd.h"  
#include "touch.h" 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"

//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void);

////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);


//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);

//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2);

//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);

////////////////////////////////////////////////////////////////////////////////
 //5个触控点的颜色(电容触摸屏用)												  
//电阻触摸屏测试函数
void rtp_test(void);

//电容触摸屏测试函数
void ctp_test(void);
