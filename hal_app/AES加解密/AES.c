#include "AES.h"
#include "track_fun_AES.h"
/*----------------usersetting------------*/

/*----------------end usersetting------------*/
unsigned char Use_AES = AES_DISABLR;

unsigned char AES128key[16] = "123456789abcdefa";//秘钥
unsigned char AES_IV[16]= "0102030405123456";//向量表
//unsigned char AES128expKey[4 * Nc * (Nr + 1)];  //初始化密钥
void AES_UART1_SendData(unsigned char *src,unsigned char len)
{
    //unsigned char AES_Len;
    unsigned char buf[256];
    //unsigned char out[16];//密文输出
    unsigned char in[48] = {0};
    unsigned char i;
    if(Use_AES == AES_ENABLE)
    {
        AES_Init(AES128key);//AES初始化
        for (i=0;i<len-3;i++)
        {
        in[i] = src[i+2];
        }
         
        buf[0] = 0xBA;
        buf[1] = ((len-3)/16+1)*16;
        AES_Encrypt(in, buf+2, buf[1], AES_IV);//加密
        //AES_Decrypt(buf+2, buf+2, buf[1], AES_IV);//解密
        buf[buf[1]+2] = HCGetCheckSum(buf, buf[1]+2);
        UART1_SendData(buf, buf[1]+3);
    }
    else 
    {
        UART1_SendData(src, len);
    }
}
// 串口解密
//串口数据过滤
unsigned char AES_UART_DECODE(void)
{
    unsigned char len=0;
    if(cUart1RxData[0] == 0xBA)
    {
        if((cUart1RxData[1]%16)==0)
        {
            len = cUart1RxData[1];
            AES_Decrypt(cUart1RxData+2, cUart1RxData+2, len, AES_IV);//解密
            
            cUart1RxData[0] = 0xBB;//转换帧头
            cUart1RxData[1] = cUart1RxData[3] + 2;// 重新算长度
            cUart1RxData[cUart1RxData[1]+ 2] = HCGetCheckSum(cUart1RxData, cUart1RxData[1]+ 2);//从新算校验
            return 1;//h加密
        }
        else 
        {
            return 2;//加密信息长度错误
        }
    }
    else 
    {
        return 0;//没加密
    }
}