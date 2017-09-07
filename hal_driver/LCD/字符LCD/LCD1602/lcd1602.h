#ifndef __LCD1602_H__
#define __LCD1602_H__			 
#include "main.h"	 	
#include "gpio.h"

#define LCD_BK  PBout(5)
#define LCD_CE  PAout(13)
#define LCD_RW  PAout(12)
#define LCD_RS  PAout(11)
#define LCD_DB7 PBout(4) 
#define LCD_DB6 PBout(3)
#define LCD_DB5 PDout(2)
#define LCD_DB4 PCout(12)
#define LCD_DB3 PCout(11)
#define LCD_DB2 PCout(10)
#define LCD_DB1 PAout(15)
#define LCD_DB0 PAout(14)

#define LCD_BK_ON  LCD_BK=0;
#define LCD_BK_OFF LCD_BK=1;

extern void lcd1602_Reset(void); //复位lcd1602 
extern void lcd1602_DispOneChar(u8 x,u8 y,u8 Wdata);//在指定位置显示一个字符//x:0~15 //y:0~1
extern void lcd1602_cls(void);    //清屏
extern void lcd1602_Display(u8 dd);  //显示       
extern void lcd1602_LocateXY(u8 posx,u8 posy);  //显示光标位置
extern void lcd1602_ePutstr(u8 x,u8 y,char *ptr);
extern void lcd1602_init(void);


#endif	

