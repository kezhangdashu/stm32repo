/**********************************************************/
//公司名称：
//创建时间：2017-7-10
//作者：ke
//文件名：esp8266_io.c
//功能描述：esp8266模块io操作驱动	
/**********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "esp8266_io.h"
#include "gpio.h"
#include "delay.h"
#include "mytimer.h"
#include "malloc.h"	   

#define WIFI_RST_IO PAout(4)          // PA4
//#define WIFI_MODE_IO PAout(15)      // PA15	


TIMER tuart3_ov;    //接收超时定时器

u8 *uart3_rxbuf;		              //定义串口接收缓冲区
u8 *uart3_txbuf;                      //定义串口发送缓冲区

u8 esp8266_cmd_mode = 0;


u32 uart3_rx_tim = 0,uart3_rx_cnt = 0,uart3_rx_cnt_last = 0;

typedef __packed struct _DMAFIFO
{
	u32     size;       /*缓冲区大小*/
 	u32		front;		/*读地址*/
	u32		rear;		/*写地址*/
} DMAFIFO;

DMAFIFO dmaff;

#define DMAFIFOSIZE 1024+32

u8 uart_fifo_buf[DMAFIFOSIZE];

/*******************************************************************
函数原形： void init_dma_fifo (DMAFIFO *pdfifo)
功能：  初始化DMA fifo
参数：  pdfifo要操作fifo的地址
返回：无
*******************************************************************/
void init_dma_fifo (DMAFIFO *pdfifo)
{
	pdfifo->size = DMAFIFOSIZE;
	pdfifo->front = 0;
//	pdfifo->rear = 0;
	pdfifo->rear = DMAFIFOSIZE - (uint16_t)(DMA1_Channel3->CNDTR);
}

/*******************************************************************
函数原形： u8 delete_dma_fifo(DMAFIFO* pdfifo, u8 *pelem)
功能：  数据fifo包删除；
参数：  pdfifo要操作fifo的地址，pelem从fifo中移除数据转存地址
返回：1表示fifo里面无数据
*******************************************************************/
u32 delete_dma_fifo(DMAFIFO* pdfifo, u8 *pelem)
{
	u32 len = 0;
	
	while (1)
	{
		pdfifo->rear = DMAFIFOSIZE - (uint16_t)(DMA1_Channel3->CNDTR);
	
		if (pdfifo->front == pdfifo->rear)break;								   //数据fifo空返回错误1
	
		pelem[len] = uart_fifo_buf[pdfifo->front];
//		putchar (uart_fifo_buf[pdfifo->front]);
		pdfifo->front = (pdfifo->front +1)%pdfifo->size;
		
		len++;
	}

	return len;
}


extern DMA_HandleTypeDef hdma_usart3_rx;

void DMA1_Channel3_IRQHandler(void)
{
  
//  HAL_DMA_IRQHandler(&hdma_usart3_rx);
	
	/* Clear the transfer error flag */
	 __HAL_DMA_CLEAR_FLAG(&hdma_usart3_rx, __HAL_DMA_GET_TE_FLAG_INDEX(&hdma_usart3_rx));
	
	 /* Clear the half transfer complete flag */
   __HAL_DMA_CLEAR_FLAG(&hdma_usart3_rx, __HAL_DMA_GET_HT_FLAG_INDEX(&hdma_usart3_rx));
	
	/* Clear the transfer complete flag */
   __HAL_DMA_CLEAR_FLAG(&hdma_usart3_rx, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_usart3_rx));
	
//	printf ("DMA IR instert\r\n");

}

void u3_putchar (u8 ch)
{
	while((USART3->SR&0X40)==0);
	USART3->DR = (u8) ch;
}


//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)uart3_txbuf,fmt,ap);
	va_end(ap);
	i=strlen((const char*)uart3_txbuf);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = uart3_txbuf[j];
	} 
}


/*******************************************************************
函数原形： void esp8266_dma_init(void)
功能：    DMA1通道3初始化，用于uart3接收
参数：无
*******************************************************************/
void esp8266_dma_init(void)
{
	//DMA部分设置
//	__HAL_DMA_DISABLE(&hdma_usart3_rx);                             //关闭DMA
	
//	HAL_NVIC_DisableIRQ(DMA1_Channel3_IRQn);              //关闭DMA所有中断
	
//	MYDMA_SetConfig(phuart->hdmarx, (uint32_t)&phuart->Instance->DR, (uint32_t)&uart_fifo_buf[0], DMAFIFOSIZE);  //设置DMA参数
	
	hdma_usart3_rx.Instance->CNDTR = DMAFIFOSIZE;
	hdma_usart3_rx.Instance->CPAR = (uint32_t)&huart3.Instance->DR;
	hdma_usart3_rx.Instance->CMAR = (uint32_t)uart_fifo_buf;
	
	/* Enable the transfer complete interrupt */
//  __HAL_DMA_ENABLE_IT(&hdma_usart3_rx, DMA_IT_TC);

  /* Enable the Half transfer complete interrupt */
//  __HAL_DMA_ENABLE_IT(&hdma_usart3_rx, DMA_IT_HT);  

  /* Enable the transfer Error interrupt */
//  __HAL_DMA_ENABLE_IT(&hdma_usart3_rx, DMA_IT_TE);
	
	__HAL_DMA_ENABLE(&hdma_usart3_rx);                     //开启DMA

  SET_BIT(huart3.Instance->CR3, USART_CR3_DMAR);       //Enable the DMA transfer for the receiver request by setting the DMAR bit in the UART CR3 register 
}

/*******************************************************************
函数原形： void esp8266_io_hspeed (void)
功能：设置串口到高速模式
参数：无
*******************************************************************/
void esp8266_io_hspeed (void)
{
	MY_USART3_UART_ReSet_baud (ESP8266_COM_HSPEED);
	init_dma_fifo (&dmaff);
	time_out_set (&tuart3_ov,DMA_RX_OVTIM);
	uart3_rx_cnt = 0;
	uart3_rx_cnt_last = 0;
}

/*******************************************************************
函数原形： void esp8266_io_init(void)
功能：    esp8266相关硬件初始化
参数：无
*******************************************************************/
void esp8266_io_init(void)
{
	uart3_rxbuf = mymalloc(SRAMIN,UART3_RX_DEEP);
	uart3_txbuf = mymalloc(SRAMIN,UART3_TX_DEEP);
	if ((uart3_rxbuf == NULL) || (uart3_txbuf == NULL))
	{
		printf ("esp8266 io memory\r\n");
		while (1);
	}
	init_dma_fifo (&dmaff);
	time_out_set (&tuart3_ov,DMA_RX_OVTIM);
	uart3_rx_cnt = 0;
	uart3_rx_cnt_last = 0;
	
	//硬件复位
	WIFI_RST_IO = 0;          
	delay_ms (100);
	WIFI_RST_IO = 1;
	delay_ms (30);
}

/*******************************************************************
函数原形： u32 uart3_get_data (void)
功能：    获取串口数据
参数：无
返回：接收到的数据总数
注意：该函数必须实时调用否则可能数据溢出
*******************************************************************/
u32 uart3_get_data (void)
{
	u32 res = 0,len;

	if (is_time_out (&tuart3_ov))
	{
		len = delete_dma_fifo(&dmaff, uart3_rxbuf+uart3_rx_cnt);
		uart3_rx_cnt += len;
		if ((uart3_rx_cnt != 0) && (uart3_rx_cnt_last == uart3_rx_cnt))
		{
			uart3_rxbuf[uart3_rx_cnt] = 0;   //转换成字符串
			res = uart3_rx_cnt;
			uart3_rx_cnt = 0;
			uart3_rx_cnt_last = 0;
		}
		else if (uart3_rx_cnt_last != uart3_rx_cnt)
		{
			uart3_rx_cnt_last = uart3_rx_cnt;
		}
		time_out_set (&tuart3_ov,DMA_RX_OVTIM);
	}
	return res;
}


void esp8266_test (void)
{
	u32 len,i;
	len = uart3_get_data ();
	if (len)
	{
		for (i = 0; i < len; i++)
		{
			putchar (uart3_rxbuf[i]);
//			u3_putchar (uart3_rxbuf[i]);
		}
//		uart3_rx_endma ();
	}
}


