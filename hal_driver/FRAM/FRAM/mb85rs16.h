#ifndef __MB85RS16_H__
#define __MB85RS16_H__

#include "main.h" 
#include "gpio.h"


extern u32 MB85RSXX_TYPE;					//定义芯片型号		 

#define	MB85RSXX_CS 		PBout(12)  		//片选信号

#define MB85RSXX_ID     0x047f0101

 
//指令表
#define MB85RSXX_WriteEnable		  0x06 
#define MB85RSXX_WriteReset		    0x04 
#define MB85RSXX_ReadStatusReg		0x05 
#define MB85RSXX_WriteStatusReg		0x01 
#define MB85RSXX_ReadData			    0x03 
#define MB85RSXX_WriteData        0x02
#define MB85RSXX_DeviceID		      0x9F 

extern void MB85RSXX_Init(void);
extern u32 MB85RSXX_ReadID(void);  	    		//读取FLASH ID
extern u8 MB85RSXX_ReadSR(void);          //读取状态寄存器 
extern void MB85RSXX_WriteSR(u8 sr);  			  //写状态寄存器
extern void MB85RSXX_Write_Enable(void);  		//写使能 
extern void MB85RSXX_Write_Disable(void);		//写保护
extern void MB85RSXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取数据
extern void MB85RSXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入数据

#endif

