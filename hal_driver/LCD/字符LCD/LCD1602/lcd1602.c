/**********************************************************/
//公司名称：
//创建时间：2017-8-25
//作者：ke
//文件名：lcd1206.c
//功能描述：LCD1206驱动
/**********************************************************/
#include <string.h>
#include "lcd1602.h"
#include "delay.h"

#define LCD_DAOUT(x) LCD_DB7=(x>>7)&0x01;LCD_DB6=(x>>6)&0x01;LCD_DB5=(x>>5)&0x01;LCD_DB4=(x>>4)&0x01;\
                     LCD_DB3=(x>>3)&0x01;LCD_DB2=(x>>2)&0x01;LCD_DB1=(x>>1)&0x01;LCD_DB0=x&0x01

//写入控制字。
//RS=0,
//RW=0
//E=1
void lcd1602_wrcmd(u8 cmd)
{
	delay_ms(2);  //等待控制器完成指令
	LCD_RS=0;
	LCD_RW=0;
	LCD_CE=1;
	LCD_DAOUT(cmd);
	delay_us(2);
	LCD_CE=0;               
}

//写入一个字节。
//dataW:要写入的数据/命令
//RS=1
//RW=0
//E=1
void lcd1602_wrdata(u8 data)
{
 delay_ms(2); //等待控制器完成指令
 LCD_RS=1;
 LCD_RW=0;
 LCD_CE=1;
 
 LCD_DAOUT(data);
 delay_us(2);
 LCD_CE=0;              
}

//清屏
////
void lcd1602_cls(void)
{
 lcd1602_wrcmd(0x01); //             
}
//显示
void lcd1602_Display(u8 dd)
{             
 u8 i=0;
 for(i=0;i<16;i++)
 {
  lcd1602_DispOneChar(i,1,dd++);
  dd &=0x7f;
  if (dd<32) dd=32;
 }         
}
//显示光标位置
void lcd1602_LocateXY(u8 posx,u8 posy)
{             
 u8 temp;
 temp=posx & 0xf;
 posy &= 0x01;
 if(posy) temp |=0x40;
 temp |=0x80;
 lcd1602_wrcmd(temp);         
}
//在指定位置显示一个字符
//x:0~15
//y:0~1
void lcd1602_DispOneChar(u8 x,u8 y,u8 Wdata)
{             
 lcd1602_LocateXY(x,y);
 lcd1602_wrdata(Wdata);         
}
//显示字符串
void lcd1602_ePutstr(u8 x,u8 y,char *ptr)
{             
 u8 i;
 u32 len;
 len = strlen ((char*)ptr);
 for (i=0;i<len;i++)
 {
  lcd1602_DispOneChar(x++,y,*(ptr+i));
  if ( x == 16 )
  {
//   x = 0; 
//   y ^= 1;
		return;
  }
 }         
}
//复位
////
void lcd1602_Reset(void)
{
 lcd1602_wrcmd(0x38); //
 delay_ms(15);
 lcd1602_wrcmd(0x38); //
 delay_ms(15);
 lcd1602_wrcmd(0x38); //
 delay_ms(15);
 lcd1602_wrcmd(0x38); //
 lcd1602_wrcmd(0x08); //
 lcd1602_cls(); //clear device
 lcd1602_wrcmd(0x06); //
 lcd1602_wrcmd(0x0c); //
              
}

void lcd1602_init(void)
{
	lcd1602_Reset();
	lcd1602_cls();
}

