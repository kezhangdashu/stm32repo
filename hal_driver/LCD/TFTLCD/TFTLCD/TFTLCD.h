//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : 1.8寸LCD 4接口演示例程(STM32系列)
/******************************************************************************
//本程序适用与STM32F103C8
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PA5（SCL）
//              SDA   接PA7（SDA）
//              RES   接PB0
//              DC    接PB1
//              CS    接PA4 
//							BL		接PB10
*******************************************************************************/
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//******************************************************************************/

#include "main.h"
#include "gpio.h"

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


//******************************************************************************/
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PC0
//              SDA   接PC1
//              RES   接PC2
//              DC    接PC3
//              CS    接PC4 
//							BL		接PC5
//*******************************************************************************//

#define LCD_SCL        	PCout(0)     // PC0
#define LCD_SDA        	PCout(1)     // PC1
#define LCD_CS          PCout(4)     // PC4

#define LCD_LED        	PCout(5)     // PC5
#define LCD_RS         	PCout(3)     // PC3
#define LCD_RST     	  PCout(2)     // PC2



void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_SetXY(u16 x,u16 y);

unsigned int Lcd_ReadPoint(u16 x,u16 y);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void LCD_WriteData_16Bit(u16 Data);

void Gui_DrawPoint(u16 x,u16 y,u16 Data);
void Lcd_BmpColor_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u8 *color);
void Lcd_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);

void Lcd_BmpColor_Fill2(u16 sx,u16 sy,u16 ex,u16 ey,u8 *color);

