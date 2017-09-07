/**********************************************************/
//公司名称：
//创建时间：2017-8-28
//作者：ke
//文件名：mb85rs16.c
//功能描述：fram驱动	
/**********************************************************/
#include "mb85rs16.h" 
#include "spi.h"
#include "delay.h"


u32 MB85RSXX_TYPE = 0;

/*******************************************************************
函数原形： void MB85RSXX_Init(void)
功能：    初始化FRAM
参数：无
返回：无
*******************************************************************/
void MB85RSXX_Init(void)
{	
  MB85RSXX_CS=1;
	__HAL_SPI_ENABLE(&hspi2);           //开启SPI2
	MB85RSXX_TYPE=MB85RSXX_ReadID();    //读取FLASH ID.  

} 


/*******************************************************************
函数原形： u8 MB85RSXX_ReadSR(void) 
功能：读状态寄存器
参数：无
返回：状态寄存器值
BIT7  6   5   4   3   2   1   0
WPEN             BP1 BP0 WEL
WPEN:状态寄存器写保护，默认0
BP1~BP0:块保护
WEL:写使能锁定
*******************************************************************/
u8 MB85RSXX_ReadSR(void)   
{  
	u8 byte=0;   
	MB85RSXX_CS=0;                          
	SPI2_ReadWriteByte(MB85RSXX_ReadStatusReg); //发送读取状态寄存器命令    
	byte=SPI2_ReadWriteByte(0Xff);              //读取一个字节  
	MB85RSXX_CS=1;                                
	return byte;   
} 

/*******************************************************************
函数原形： void MB85RSXX_WriteSR(u8 sr)
功能： 写MB85RSXX状态寄存器
参数：sr要写的值
返回：无
*******************************************************************/
void MB85RSXX_WriteSR(u8 sr)   
{   
	MB85RSXX_CS=0;                            //使能器件   
	SPI2_ReadWriteByte(MB85RSXX_WriteStatusReg);//发送写取状态寄存器命令    
	SPI2_ReadWriteByte(sr);               	   //写入一个字节  
	MB85RSXX_CS=1;                            //取消片选     	      
}   
 
/*******************************************************************
函数原形：void MB85RSXX_Write_Enable(void)  
功能：写使能，将WEL置位 
参数：无
返回：无
*******************************************************************/
void MB85RSXX_Write_Enable(void)   
{
	MB85RSXX_CS=0;                          	//使能器件   
	SPI2_ReadWriteByte(MB85RSXX_WriteEnable); 	//发送写使能  
	MB85RSXX_CS=1;                           	//取消片选     	      
} 
 
/*******************************************************************
函数原形：void MB85RSXX_Write_Disable(void)   
功能：写使能，将WEL清零
参数：无 
返回：无
*******************************************************************/
void MB85RSXX_Write_Disable(void)   
{  
	MB85RSXX_CS=0;                            //使能器件   
	SPI2_ReadWriteByte(MB85RSXX_WriteReset);  //发送写禁止指令    
	MB85RSXX_CS=1;                            //取消片选     	      
} 		

/*******************************************************************
函数原形：u32 MB85RSXX_ReadID(void)  
功能：读取芯片ID
参数：无 
返回：无
*******************************************************************/
u32 MB85RSXX_ReadID(void)
{
	u32 Temp = 0;	  
	MB85RSXX_CS=0;				    
	SPI2_ReadWriteByte(MB85RSXX_DeviceID);//发送读取ID命令	    	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<24;  
	Temp|=SPI2_ReadWriteByte(0xFF)<<16;	
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;	
	Temp|=SPI2_ReadWriteByte(0xFF);	
	MB85RSXX_CS=1;				    
	return Temp;
}   		    

/*******************************************************************
函数原形：void MB85RSXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
功能：读取FRAM,在指定地址开始读取指定长度的数据
参数：pBuffer数据存储区,ReadAddr开始读取的地址(24bit),NumByteToRead要读取的字节数(最大65535)
返回：无
注意：没有检查地址越界，使用时一定要注意
*******************************************************************/
void MB85RSXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	MB85RSXX_CS=0;                            	//使能器件   
	SPI2_ReadWriteByte(MB85RSXX_ReadData);         	//发送读取命令   
  SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  	//发送24bit地址    
  SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
  SPI2_ReadWriteByte((u8)ReadAddr);   
  for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=SPI2_ReadWriteByte(0XFF);   	//循环读数  
  }
	MB85RSXX_CS=1;  				    	      
}  

/*******************************************************************
函数原形：void MB85RSXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
功能：写入FRAM,在指定地址开始写入指定长度的数据
参数：pBuffer数据存储区,WriteAddr开始写入的地址(24bit),NumByteToWrite要写入的字节数(最大65535)
返回：无
注意：没有检查地址越界，使用时一定要注意
*******************************************************************/
void MB85RSXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  MB85RSXX_Write_Enable();                  	//SET WEL 
	MB85RSXX_CS=0;                            	//使能器件   
  SPI2_ReadWriteByte(MB85RSXX_WriteData);      	//发送写命令   
  SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); 	//发送24bit地址    
  SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI2_ReadWriteByte((u8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)
	{
		SPI2_ReadWriteByte(pBuffer[i]);//循环写数  
	}
	MB85RSXX_CS=1;                            	//取消片选 
}


