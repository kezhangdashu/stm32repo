/**********************************************************/
//公司名称：
//创建时间：2017-8-17
//作者：ke
//文件名：dmx512.c
//功能描述：DMX512驱动
//注意：采用串口中断方式接收
/**********************************************************/
#include <string.h>	
#include "cmd.h"
#include "usart.h"
#include "dmx512.h"	 
#include "delay.h"
#include "mytimer.h"

//模式控制
#define RS485_TX_EN		PAout(8)	//485模式控制.1,接收;0,发送.

#define DMXFIFOSIZE 516       //缓冲区大小
#define DMXLOSTTIM  500        //DMX超时时间    

u8 dmxfifo_buf[DMXFIFOSIZE];       //如果使用DMA接收缓冲区要使用16位的
u32 dmx_cnt = 0;            //DMX接收单帧字节数
u32 dmx_frame_cnt = 0;      //DMX接收总字节数      
u32 dmx_frame_cnt_last = 0; //DMX最后接收总字节数
u8 dmx_lost = 1;            //DMX信号丢失
TIMER tdmx_over;

u8 DMX_ADC[3] = {0,0,0};



u16 urx_temp;

void USART1_IRQHandler(void)                	//串口1中断服务程序
{ 
	if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
  {
		urx_temp = huart1.Instance->DR;
		if (urx_temp & 0x100)              //数据
		{
			if (dmx_cnt <= 513)
			{
				dmx_cnt++;
				dmxfifo_buf [dmx_cnt] = urx_temp;
			}
		}
		else if (urx_temp == 0x0000)          //break信号
	  {
			dmx_cnt = 0;
			dmxfifo_buf [dmx_cnt] = urx_temp;
			dmx_frame_cnt++;                  
		}
	}
} 


							 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void DMX_init(void)
{  
  RS485_TX_EN=1;			//默认为接收模式
	
	dmx_cnt = 0;
	dmx_frame_cnt = 0;      //DMX帧数      
	dmx_frame_cnt_last = 0; //DMX最后帧数
	dmx_lost = 1;
	
	time_out_set (&tdmx_over,DMXLOSTTIM);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);       //使能接收中断
}

/*******************************************************************
函数原形： void DMX_get (u8 dmxc)
功能：DMX获取指定通道数据
参数：dmxc通道号0~511
返回：无
*******************************************************************/
void DMX_get (u8 dmxc)
{
	DMX_ADC [0] = dmxfifo_buf [2+dmxc];
	DMX_ADC [1] = dmxfifo_buf [2+dmxc+1];
	DMX_ADC [2] = dmxfifo_buf [2+dmxc+2];

	
	if (dmx_frame_cnt != dmx_frame_cnt_last)
	{
		dmx_lost = 0;
		dmx_frame_cnt_last = dmx_frame_cnt;
		time_out_set (&tdmx_over,DMXLOSTTIM);
	}
	else if (is_time_out (&tdmx_over))
	{
		dmx_lost = 1;
//		time_out_set (&tdmx_over,DMXLOSTTIM);
	}
}


