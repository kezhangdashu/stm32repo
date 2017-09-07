#ifndef __MYIIC_H
#define __MYIIC_H
#include "main.h"
#include "gpio.h"

//IO方向设置
//#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
//#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
#define SDA_IN()  {GPIOB->CRL&=0XFFF0FFFF;GPIOB->CRL|=0X00080000;}
#define SDA_OUT() {GPIOB->CRL&=0XFFF0FFFF;GPIOB->CRL|=0X00030000;}
                                
//IO操作函数	 
#define IIC_SCL    PBout(5) //SCL
#define IIC_SDA    PBout(4) //SDA	 
#define READ_SDA   PBin(4)  //输入SDA 

//IIC所有操作函数
//void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	
				 
//void IICLOW_Start(void);				//发送IIC开始信号
//void IICLOW_Stop(void);	  			//发送IIC停止信号
//void IICLOW_Send_Byte(u8 txd);			//IIC发送一个字节
//u8 IICLOW_Read_Byte(unsigned char ack);//IIC读取一个字节
//u8 IICLOW_Wait_Ack(void); 				//IIC等待ACK信号
//void IICLOW_Ack(void);					//IIC发送ACK信号
//void IICLOW_NAck(void);				//IIC不发送ACK信号

//void IICLOW_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 IICLOW_Read_One_Byte(u8 daddr,u8 addr);

#endif
















