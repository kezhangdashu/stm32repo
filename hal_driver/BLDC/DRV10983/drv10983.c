#include <stdio.h>
#include <string.h>
#include "myiic.h"
#include "drv10983.h"
#include "delay.h"

#define DRV10983_ADDR 0xa4

const u8 ge_ConfigRegAddress[NUMCONFIGREGS] ={	
	0x20, 0x21, 0x22, 0X23,0x24, 0x25, 0x26, 0x27,0x28, 0x29, 0x2A, 0x2B
};

u8 ge_ConfigRegData[NUMCONFIGREGS] = {
	REG_20_DATA,REG_21_DATA,REG_22_DATA,REG_23_DATA,REG_24_DATA,REG_25_DATA,
	REG_26_DATA,REG_27_DATA,REG_28_DATA,REG_29_DATA,REG_2A_DATA,REG_2B_DATA
};

//u8 ge_Reg_Read_Data[NUMCONFIGREGS];


//初始化IIC接口
//void DRV10983_io_init(void)
//{
//	IIC_Init();
//}
//在DRV10983指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 DRV10983_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;
	
  IIC_Start();  
	IIC_Send_Byte(DRV10983_ADDR);   //发送器件地址,写数据 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(DRV10983_ADDR+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在DRV10983指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void DRV10983_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	IIC_Send_Byte(DRV10983_ADDR);   //发送器件地址DRV10983_ADDR,写数据 	 
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();//产生一个停止条件 
//	delay_ms(10);	 
}
//在DRV10983里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void DRV10983_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		DRV10983_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在DRV10983里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 DRV10983_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=DRV10983_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

//在DRV10983里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void DRV10983_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=DRV10983_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在DRV10983里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void DRV10983_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		DRV10983_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}


/**********************DRV10983_75_supplyVoltage******************************/
/* 	This function continues to read register 0x1A where the supply voltage is
 * 	stored in the lower byte.
 * 	for programming the DRV10983 and the DRV10975, the device must have a
 * 	supply voltage above 22V
 * 	the supply voltage is calculated differently for DRV10983 and for DRV10975
 *
 * 	For DRV10983 SupplyVoltage[7:0] ?30 V /255.
 * 	so 0xBB ?30 / 255 = 12 V
 *
 * 	For DRV10975 SupplyVoltage[7:0] ?22.8 V /255.
 * 	so 0xF0 ?22.8 / 255 = 22 V
 *
 * 	This function will return only when the supply voltage is over 22V
 */
//void DRV10983_75_supplyVoltage(void)
//{
//	u8 sV_data = 0x00;
//	u8 sV_device = 0x00;

//	sV_device =  DRV10983_ReadOneByte(DRV_83_OR_75);
//	sV_device = sV_device & MASK_83_OR_75;

//	if(0x00 == sV_device)
//	{
//		while(SUPPLY_V_22_83 > sV_data)							/* checking for voltage to be above 12V 				*/
//		{
//			sV_data = DRV10983_ReadOneByte(SUPPLY_VOLTAGE);		/* read reg 0x1A for supply voltage store value		 */
//		}
//	}
//	else
//	{
//		while(SUPPLY_V_22_75 > sV_data)							/* checking for voltage to be above 12V 				*/
//		{
//			sV_data = DRV10983_ReadOneByte(SUPPLY_VOLTAGE);		/* read reg 0x1A for supply voltage store value		 */
//		}
//	}
//}

int DRV10983_75_supplyVoltage(void)
{
	u8 sV_data = 0x00;
	u32 i = 0;
	
	for (i = 0; i < 10; i++)
	{
		sV_data = DRV10983_ReadOneByte(SUPPLY_VOLTAGE);		/* read reg 0x1A for supply voltage store value		 */
		if (sV_data > SUPPLY_V_22_83)return 0;            /* checking for voltage to be above 22V 				*/
		delay_ms (100);
	}
		
	return -1;
}

/**********************DRV10983_75_refresheePromRegs******************************/
/*	This function sets the eeRefresh bit
 *
 * The device has a bit, 慹eRefresh? which loads the registers with the
 * 	values in EEPROM. Setting 慹eRefresh?has the same effect as power cycling
 * 	the device. After the bit is set to 1, the registers are loaded with the
 * 	values stored in EEPROM, then the device clears the bit
 */
void DRV10983_75_refresheePromRegs(void)
{
	DRV10983_WriteOneByte(EE_CTRL, EE_REFRESH_1);
	delay_ms(24);
}

/**********************DRV10983_75_setSIData******************************/
/*	This function sets the SI bit
 *
 * If this bit is not enabled, the register values cannot change. Set the
 * Sidata bit to '1' one time before setting all of the register values.
 */
void DRV10983_75_setSIData(void)
{
	DRV10983_WriteOneByte(EE_CTRL, SI_DATA_1);
}

/**********************DRV10983_75_starteePromProgramming******************************/
/*	After the registers are loaded, the next step is to write them to
 * 	EEPROM so the device can maintain those settings. First, enter the program key
 * 	(0xB6) in the device control register (0x02), and then immediately
 * 	after, set the eeWrite bit to 1 (0x50) in the EEPROM control register
 * 	(0x03). If the eeWrite bit is not set directly after the program key
 * 	is entered, the program key will be reset.The programming time is
 * 	about 24 ms, and when finished, the device clears the eeWrite bit.
 * 	After the data is stored in EEPROM, the device can be powered down,
 * 	and upon power-up, it auto loads the values into the registers. For the
 * 	device to properly write to EEPROM, the VCC must be at least 22 V and
 * 	have at least a 24-ms delay before power cycling or refreshing the device.
 */
//void DRV10983_75_starteePromProgramming(void)
//{
//	u8 ge_sePP_data = EE_WRITE_1;

//	DRV10983_75_supplyVoltage();
//	DRV10983_WriteOneByte(DEV_CTRL, EN_PROG_KEY);
//	DRV10983_WriteOneByte(EE_CTRL, EE_WRITE_1 | SI_DATA_1);
//	delay_ms(24);

//	while(EE_WRITE_0 != ge_sePP_data)
//	{
//		ge_sePP_data = DRV10983_ReadOneByte(EE_CTRL);
//		ge_sePP_data= ge_sePP_data & EE_WRITE_1;
//	}
//}
int DRV10983_75_starteePromProgramming(void)
{
	u8 ge_sePP_data = EE_WRITE_1;

	if (DRV10983_75_supplyVoltage() != 0)return -1;
	
	DRV10983_WriteOneByte(DEV_CTRL, EN_PROG_KEY);
	DRV10983_WriteOneByte(EE_CTRL, EE_WRITE_1 | SI_DATA_1);
	delay_ms(24);

	while(EE_WRITE_0 != ge_sePP_data)
	{
		ge_sePP_data = DRV10983_ReadOneByte(EE_CTRL);
		ge_sePP_data= ge_sePP_data & EE_WRITE_1;
	}
	return 0;
}

/**********************DRV10983_75_massRead******************************/
/*	This function loops through all 12 configuration registers in the
 * 	DRV10983 or DRV10975 and reads them this function should only be called after
 * 	the EEPROM data has been loaded into the registers
 */
void DRV10964_massRead(void)
{
	u8 mR_i = 0;

	for(mR_i = 0; mR_i < NUMCONFIGREGS; mR_i++)
	{
		ge_ConfigRegData[mR_i] = DRV10983_ReadOneByte(ge_ConfigRegAddress[mR_i]);
	}
}

/**********************DRV10983_75_massWrite******************************/
/*	This function loops through all 12 configuration registers in the
 * 	DRV10983 or DRV10975 and writes to them with the designated data in
 * 	the header file
 */
void DRV10964_massWrite(void)
{
	u8 mR_i = 0;

	for(mR_i = 0; mR_i < NUMCONFIGREGS; mR_i++)
	{
		DRV10983_WriteOneByte(ge_ConfigRegAddress[mR_i], ge_ConfigRegData[mR_i]);
		printf ("program 0x%2.2x = 0x%2.2x\r\n",ge_ConfigRegAddress[mR_i], ge_ConfigRegData[mR_i]);
	}
}

/*******************************************************************
函数原形： void DRV10983_75eePromRegs_write(void)
功能：编程DRV10983参数EEPROM
参数：无
返回：0成功，其他值失败
*******************************************************************/
int DRV10983_75eePromRegs_write(void)
{
	if (DRV10983_75_supplyVoltage() != 0)
	{
		printf ("Prom error1 supplyVoltage < 22V\r\n");
		return -1;
	}
	DRV10983_75_setSIData();
	DRV10964_massWrite();
	if (DRV10983_75_starteePromProgramming() != 0)
	{
		printf ("Prom error2 supplyVoltage < 22V\r\n");
		return -1;
	}
	return 0;
}

/*******************************************************************
函数原形： void DRV10983_75eePromRegs_read(void)
功能：读DRV10983参数EEPROM
参数：无
返回：无
*******************************************************************/
void DRV10983_75eePromRegs_read(void)
{
	u8 i;
	
	DRV10964_massRead();
	for (i = 0; i < NUMCONFIGREGS; i++)
	{
		printf ("DVR10983 reg %2.2x = %2.2x\r\n",0x20+i,ge_ConfigRegData[i]);
	}
}

/*******************************************************************
函数原形： void DRV10983_wrspeed (u16 speed)
功能：写速度寄存器
参数：speed速度值，范围0~511
返回：无
*******************************************************************/
void DRV10983_wrspeed (u16 speed)
{
	u8 temp = (speed>>8)&0x01;
	
	if (speed > 511)return;
	
	DRV10983_WriteOneByte(SPEED_CTRL2,OVER_RIDE_1|temp);
	DRV10983_WriteOneByte(SPEED_CTRL1,speed);
	
//	printf ("SPEED_CTRL2 = 0x%2.2x\r\n",DRV10983_ReadOneByte(SPEED_CTRL2));
//	printf ("SPEED_CTRL1 = 0x%2.2x\r\n",DRV10983_ReadOneByte(SPEED_CTRL1));
}

/*******************************************************************
函数原形： u32 DRV10983_rdspeed (void)
功能：读取当前电机转速
参数：无
返回：转速单位RPM
*******************************************************************/
u32 DRV10983_rdspeed (void)
{
	u32 speed;
	
	speed = DRV10983_ReadOneByte(MOTOR_SPEED1);
  speed = (speed<<8) + DRV10983_ReadOneByte(MOTOR_SPEED2);
	speed = speed*3;          //转换成转速
	return speed;
}

/*******************************************************************
函数原形： int DRV10983_init (void)
功能：初始化DRV10983
参数：无
返回：0成功
*******************************************************************/
int DRV10983_init (void)
{

	DRV10983_WriteOneByte(SPEED_CTRL2,OVER_RIDE_1);          //设置I2C控制速度
	DRV10983_WriteOneByte(SPEED_CTRL1,0x0a);
	
	if (DRV10983_ReadOneByte(SPEED_CTRL1) != 0x0a)return -1;
	
//	DRV10964_massRead();
//	
//	if (memcmp (ge_Reg_Read_Data, ge_ConfigRegData,NUMCONFIGREGS) != 0)
//	{
//		printf ("DRV10983 EEPROM need program!!\r\n");
//		DRV10983_75eePromRegs_write();
//	}
	
	DRV10983_wrspeed (00);             //初始化速度
	
	return 0;
}

