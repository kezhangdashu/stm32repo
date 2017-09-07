﻿#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 0        //使能FLASH写入(0，不是能;1，使能)	

#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif	

											
extern u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
extern void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
extern void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
extern void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据


#endif

