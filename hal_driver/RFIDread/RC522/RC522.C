#include "RC522.h"
#include <string.h> 
#include <stdio.h>
#include "delay.h"
#include "cmd.h"
#include "spi.h"
#include "mytimer.h"
#include "message.h"

#define MAXRLEN 18

rc522_dev g_rc522_dev;             //全局rc522结构指针

const unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};           //出厂时默认密匙
const unsigned char MyKeyA[6] =     {0x20, 0x17, 0x08, 0x09, 0x15, 0x49};					  //A密匙
const unsigned char MyKeyB[6] =     {0x00, 0x00, 0x20, 0x17, 0x08, 0x09};           //B密匙

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
int PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   int status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
//  unsigned char xTest ;
   ClearBitMask(Status2Reg,0x08);
   WriteRawRC(BitFramingReg,0x07);

//  xTest = ReadRawRC(BitFramingReg);
//  if(xTest == 0x07 )
 //   { LED_GREEN  =0 ;}
 // else {LED_GREEN =1 ;while(1){}}
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//     if(status  == MI_OK )
//   { LED_GREEN  =0 ;}
//   else {LED_GREEN =1 ;}
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
int PcdAnticoll(unsigned char *pSnr)
{
    int status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
int PcdSelect(unsigned char *pSnr)
{
    int status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
int PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
int PcdRead(unsigned char addr,unsigned char *pData)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
int PcdWrite(unsigned char addr,unsigned char *pData)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}



/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
int PcdHalt(void)
{
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
    MF522_RST=1;
    delay_10ms (1);
    MF522_RST=0;
    delay_10ms (1);
    MF522_RST=1;
    delay_10ms (1);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_10ms (1);
    
    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);     
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
int M500PcdConfigISOType(unsigned char type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);

 /*     WriteRawRC(CommandReg,0x20);    //as default   
       WriteRawRC(ComIEnReg,0x80);     //as default
       WriteRawRC(DivlEnReg,0x0);      //as default
	   WriteRawRC(ComIrqReg,0x04);     //as default
	   WriteRawRC(DivIrqReg,0x0);      //as default
	   WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
	   WriteRawRC(WaterLevelReg,0x08); //as default
       WriteRawRC(ControlReg,0x20);    //as default
	   WriteRawRC(CollReg,0x80);    //as default
*/
       WriteRawRC(ModeReg,0x3D);//3F
/*	   WriteRawRC(TxModeReg,0x0);      //as default???
	   WriteRawRC(RxModeReg,0x0);      //as default???
	   WriteRawRC(TxControlReg,0x80);  //as default???

	   WriteRawRC(TxSelReg,0x10);      //as default???
   */
       WriteRawRC(RxSelReg,0x86);//84
 //      WriteRawRC(RxThresholdReg,0x84);//as default
 //      WriteRawRC(DemodReg,0x4D);      //as default

 //      WriteRawRC(ModWidthReg,0x13);//26
       WriteRawRC(RFCfgReg,0x7F);   //4F
	/*   WriteRawRC(GsNReg,0x88);        //as default???
	   WriteRawRC(CWGsCfgReg,0x20);    //as default???
       WriteRawRC(ModGsCfgReg,0x20);   //as default???
*/
   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   

  //     PcdSetTmo(106);
	    		delay_10ms(1);
       PcdAntennaOn();
   }
   else{ return -1; }
   
   return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
//unsigned char ReadRawRC(unsigned char Address)
//{
//     unsigned char i, ucAddr;
//     unsigned char ucResult=0;

//     MF522_SCK = 0;
//     MF522_NSS = 0;
//     ucAddr = ((Address<<1)&0x7E)|0x80;

//     for(i=8;i>0;i--)
//     {
//         MF522_SI = ((ucAddr&0x80)==0x80);
//         MF522_SCK = 1;
//         ucAddr <<= 1;
//         MF522_SCK = 0;
//     }

//     for(i=8;i>0;i--)
//     {
//         MF522_SCK = 1;
//         ucResult <<= 1;
//         ucResult|=MF522_SO;
//         MF522_SCK = 0;
//     }

//     MF522_NSS = 1;
//     MF522_SCK = 1;
//     return ucResult;
//}

unsigned char ReadRawRC(unsigned char Address)
{
     unsigned char ucAddr;
     unsigned char ucResult=0;
	
		 SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);
     MF522_NSS = 0;
     ucAddr = ((Address<<1)&0x7E)|0x80;

		 SPI2_ReadWriteByte(ucAddr); 
     ucResult = SPI2_ReadWriteByte(0xff);
   
     MF522_NSS = 1;
		 SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2);
     return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
//void WriteRawRC(unsigned char Address, unsigned char value)
//{  
//    unsigned char i, ucAddr;

//    MF522_SCK = 0;
//    MF522_NSS = 0;
//    ucAddr = ((Address<<1)&0x7E);

//    for(i=8;i>0;i--)
//    {
//        MF522_SI = ((ucAddr&0x80)==0x80);
//        MF522_SCK = 1;
//        ucAddr <<= 1;
//        MF522_SCK = 0;
//    }

//    for(i=8;i>0;i--)
//    {
//        MF522_SI = ((value&0x80)==0x80);
//        MF522_SCK = 1;
//        value <<= 1;
//        MF522_SCK = 0;
//    }
//    MF522_NSS = 1;
//    MF522_SCK = 1;
//}
void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char ucAddr;

		SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);
    MF522_NSS = 0;
    ucAddr = ((Address<<1)&0x7E);

    SPI2_ReadWriteByte(ucAddr); 
    SPI2_ReadWriteByte(value);
	
    MF522_NSS = 1;
		SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2);
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
int PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    int status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }
    
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
			i = 2000;
//		    i = 4000;
    do 
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE); 
   return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}


/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
int PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    int status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
 //       for (i=0; i<16; i++)
 //       {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
int PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    int status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}


///////////////////////////////////////////////////////////////////////
// Delay 10ms
///////////////////////////////////////////////////////////////////////
void delay_10ms(unsigned int _10ms)
{
	delay_ms (10);
}

/*******************************************************************
函数原形： void RC522_init (void)
功能：初始化RC522
*******************************************************************/
void RC522_init (void)
{
	MF522_NSS = 1;
	MF522_RST = 1;
	__HAL_SPI_ENABLE(&hspi2);       //开启SPI2
}

/*******************************************************************
函数原形： int RC522_ready (void)
功能：操作RC522前的准备工作
返回：0表示没有问题，否则有问题
*******************************************************************/
int RC522_ready (void)
{
	u8 vesion;
	

	memset((u8*)&g_rc522_dev,0,sizeof(rc522_dev));

	PcdReset();
	
	vesion = ReadRawRC(VersionReg); 
	
	DEBUG ("Read RC522 VersionReg = 0x%x\r\n",vesion);
	
	if (vesion == 0xff)return -1;
	
  PcdAntennaOff();
	delay_ms (10);
  PcdAntennaOn();
	delay_ms (10);
	M500PcdConfigISOType('A');
	
	time_out_set (&g_rc522_dev.tscan_fre,0);
	
	return 0;
}

/*******************************************************************
函数原形： void RC522_setc_block (u8 bknum, u8 coder, u8 *pv)
功能：设置0～3块的访问模式，具体设置看数据手册
参数：bknum块序号0～3，coder访问代码，pv控制字节
注意：使用时一定要谨慎使用否则会造成部分块无法造作
*******************************************************************/
void RC522_setc_block (u8 bknum, u8 coder, u8 *pv)
{
	u8 coder_b = ~coder;
	
	coder_b &= 0x07;
	
	if ((bknum > 3) || (coder > 7))return;
	
	pv[0] &= ~((1<<(bknum+4))|(1<<bknum));
	pv[1] &= ~((1<<(bknum+4))|(1<<bknum));
	pv[2] &= ~((1<<(bknum+4))|(1<<bknum));
	
	pv[0] |= (((coder_b>>1)&0x01)<<(bknum+4))|(((coder_b>>2)&0x01)<<bknum);
	pv[1] |= (((coder>>2)&0x01)<<(bknum+4))|(((coder_b>>0)&0x01)<<bknum);
	pv[2] |= (((coder>>0)&0x01)<<(bknum+4))|(((coder>>1)&0x01)<<bknum);
	
	pv[3] = 0x69;          //第四字节默认不使用，使用默认值0x69
}



//const u8 twdata[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
const u8 twdata[16] = {0x02,0x58,0x17,0x08,0x22,0x98,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a};


/*******************************************************************
函数原形： int RC522_card_req (void)
功能：寻卡
参数：无
返回：0成功，其他值失败
*******************************************************************/
int RC522_card_req (void)
{
	int res;
	rc522_dev *dev = &g_rc522_dev;
	u8 *rxbuf = dev->rxbuf;
	u8 *selesnr = dev->selesnr;
	
	// 寻卡，防冲突，选择卡    返回卡类型（2 bytes）+ 卡系列号(4 bytes)
	res = PcdRequest(PICC_REQIDL, rxbuf);       		//寻天线区内未进入休眠状态
//	res = PcdRequest (PICC_REQALL,rxbuf);       		//寻天线区内未进入休眠状态
	if (res == MI_OK)
	{ 
		dev->card_type = (rxbuf[0]<<8)|rxbuf[1];
		DEBUG ("pTagType = %4.4x\r\n",dev->card_type);
				
		res = PcdAnticoll(rxbuf);                         // 防冲突 读卡的系列号
		if(res == MI_OK)
		{
			memcpy(selesnr,rxbuf,4);
			DEBUG ("RFID sn = %2.2x%2.2x%2.2x%2.2x\r\n",selesnr[0],selesnr[1],selesnr[2],selesnr[3]);
			res = PcdSelect(selesnr);                         //选择卡
			if (res == MI_OK)
			{    
				DEBUG ("PcdSelect ok\r\n");
//				PcdHalt();   //命令卡片进入休眠状态
			}
			else
			{
				DEBUG ("PcdSelect error = %d\r\n",res);
			}
		}
		else
		{
				DEBUG ("PcdAnticoll error = %d\r\n",res);
		}
	}
	return res;
}

/*******************************************************************
函数原形： int RC522_card_init (void)
功能：初始化卡
参数：无
返回：0成功，其他值失败
*******************************************************************/
int RC522_card_init (void)
{
	int res;
	rc522_dev *dev = &g_rc522_dev;
	u8 *rxbuf = dev->rxbuf;
	u8 *txbuf = dev->txbuf;
	u8 *selesnr = dev->selesnr;

	
	res = PcdAuthState(PICC_AUTHENT1A,MF522_USE_CNADDR,(u8*)DefaultKey,selesnr);        //先使用默认密匙验证
	if (res != MI_OK)                                          
	{    
		DEBUG ("Card use KeyB\r\n");
		PcdHalt();  
		RC522_card_req ();
		res = PcdAuthState(PICC_AUTHENT1B,MF522_USE_CNADDR,(u8*)MyKeyB,selesnr);        //使用密匙B验证
		if (res != MI_OK)
		{
			DEBUG ("PcdAuthState error = %d\r\n",res);
			DEBUG ("This card has been initialized or is occupied!!\r\n");
			PcdHalt();
			return -1;
		}
	}
	
	res = PcdRead(MF522_USE_CNADDR, rxbuf);
	if (res == MI_OK)
	{
		DEBUG ("CARD curr data:\r\n");
		cmd_putascii (rxbuf, 16);
	}
		
	memcpy(txbuf,MyKeyA,6);
	RC522_setc_block (0,0,txbuf+6);
	RC522_setc_block (1,0,txbuf+6);
	RC522_setc_block (2,0,txbuf+6);
	RC522_setc_block (3,1,txbuf+6);
	memcpy(txbuf+10,MyKeyB,6);
	
	DEBUG ("PcdWrite data:\r\n");
	cmd_putascii (txbuf, 16);

	res = PcdWrite(MF522_USE_CNADDR, txbuf);
	if (res != MI_OK)
	{    
		DEBUG ("PcdWrite error = %d\r\n",res); 
		PcdHalt();
		return -2;		
	}
	else
	{
		DEBUG ("PcdWrite ok\r\n");
		res = PcdRead(MF522_USE_CNADDR, rxbuf);
		if (res == MI_OK)
		{
			DEBUG ("PcdRead data:\r\n");
			cmd_putascii (rxbuf, 16);
		}
	}
	
	PcdHalt();   //命令卡片进入休眠状态
	
	DEBUG ("Card init ok\r\n");
	
	return 0;
}

/*******************************************************************
函数原形： int RC522_card_write (void)
功能：写卡
参数：无
返回：0成功，其他值失败
*******************************************************************/
int RC522_card_write (void)
{
	int res;
	rc522_dev *dev = &g_rc522_dev;
	u8 *rxbuf = dev->rxbuf;
	u8 *txbuf = dev->txbuf;
	u8 *selesnr = dev->selesnr;
	
	
	res = PcdAuthState(PICC_AUTHENT1A,MF522_USE_DAADDR1,(u8*)MyKeyA,selesnr);        //密匙验证
	if (res != MI_OK)
	{
		DEBUG ("Card use KeyB\r\n");
		PcdHalt();  
		RC522_card_req ();
		res = PcdAuthState(PICC_AUTHENT1B,MF522_USE_DAADDR1,(u8*)MyKeyB,selesnr);        //密匙验证
		if (res != MI_OK)
		{
			DEBUG ("Card use DefaultKey\r\n");
			PcdHalt();  
			RC522_card_req ();
			res = PcdAuthState(PICC_AUTHENT1B,MF522_USE_DAADDR1,(u8*)DefaultKey,selesnr);        //密匙验证
			if (res != MI_OK)
			{
				DEBUG ("PcdAuthState error = %d\r\n",res);
				DEBUG ("This card not initialized!!\r\n");
				PcdHalt();
				return -1;
			}
		}
	}
	
	memcpy(txbuf,twdata,16);
	
//	DEBUG ("PcdWrite data:\r\n");
//	cmd_putmore (txbuf, 16);
	
	res = PcdWrite(MF522_USE_DAADDR1, txbuf);
	if (res != MI_OK)
	{    
		DEBUG ("PcdWrite error = %d\r\n",res);  
		PcdHalt();
		return -2;
	}
	else
	{
		DEBUG ("PcdWrite ok\r\n");
		res = PcdRead(MF522_USE_DAADDR1, rxbuf);
		if (res == MI_OK)
		{
			DEBUG ("PcdRead data:\r\n");
			cmd_putascii (rxbuf, 16);
		}
	}
	
	PcdHalt();   //命令卡片进入休眠状态
	
	DEBUG ("Card write ok\r\n");
	
	return 0;
}

/*******************************************************************
函数原形： int RC522_card_read (void)
功能：读卡
参数：无
返回：0成功，其他值失败
*******************************************************************/
int RC522_card_read (void)
{
	int res;
	rc522_dev *dev = &g_rc522_dev;
	u8 *rxbuf = dev->rxbuf;
//	u8 *txbuf = dev->txbuf;
	u8 *selesnr = dev->selesnr;
	
	res = PcdAuthState(PICC_AUTHENT1A,MF522_USE_DAADDR1,(u8*)MyKeyA,selesnr);        //密匙验证
	if (res != MI_OK)
	{   
		DEBUG ("Card use KeyB\r\n");
		PcdHalt();   
		RC522_card_req ();
		res = PcdAuthState(PICC_AUTHENT1B,MF522_USE_DAADDR1,(u8*)MyKeyB,selesnr);        //密匙验证
		if (res != MI_OK)
		{
			DEBUG ("Card use DefaultKey\r\n");
			PcdHalt();  
			RC522_card_req ();
			res = PcdAuthState(PICC_AUTHENT1B,MF522_USE_DAADDR1,(u8*)DefaultKey,selesnr);        //密匙验证
			if (res != MI_OK)
			{
				DEBUG ("PcdAuthState error = %d\r\n",res);
				DEBUG ("card verifly fail!!\r\n");
				PcdHalt();
				return -1;
			}
		}
	}
	
	res = PcdRead(MF522_USE_DAADDR1, rxbuf);
	if (res == MI_OK)
	{
		DEBUG ("PcdRead data:\r\n");
		cmd_putascii (rxbuf, 16);
	}
	
	PcdHalt();   //命令卡片进入休眠状态
	
	DEBUG ("Card read ok\r\n");
	
	return 0;
}

/*******************************************************************
函数原形： void RC522_event_get (void)
功能：RFID卡读写操作运行函数，必须实时调用
参数：无
*******************************************************************/
void RC522_event_get (void)
{
	int res;
//	u8 event;
	rc522_dev *dev = &g_rc522_dev;
//	u8 *rxbuf = dev->rxbuf;
//	u8 *txbuf = dev->txbuf;
//	u8 *selesnr = dev->selesnr;

	if (is_time_out (&dev->tscan_fre))
	{
			res = RC522_card_req ();       		//寻卡
			if (res == MI_OK)
			{ 
//				event = message_get(SYS_MSG_FIFO_DEV_CTRL);
//				if (event == CD_MSG_INIT)
//				{
//					RC522_card_init ();
//				}
//				else if (event == CD_MSG_WR)
//				{
//					RC522_card_write ();
//				}
//				else
//				{
					RC522_card_read ();
//				}
			}
			time_out_set (&dev->tscan_fre,MF522_SCAN_TIM);
	}
}

