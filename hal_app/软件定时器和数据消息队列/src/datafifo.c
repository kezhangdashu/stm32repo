/**********************************************************/
//公司名称：
//创建时间：2012-10-16
//作者：ke
//文件名：datafifo.c
//功能描述：该文件提供一个先进先出的缓冲区	
/**********************************************************/
#include "main.h"
#include "datafifo.h"
#include <stdio.h>


#ifdef SUPDYMEM
#include "malloc.h"
/*******************************************************************
函数原形： u8 init_data_fifo (DATAFIFO* pdatafifo, u32 set_size)
功能：  	初始化数据fifo相关参数；
参数：     set_size要申请的fifo大小
*******************************************************************/
DATAFIFO* init_data_fifo (u32 set_size)
{
	DATAFIFO* pdfifo;
	
	if ((set_size < 1) || (set_size > 4096))
	{
		printf ("Datafifo set_size error!!\r\n");
		return NULL;
	}
	
	pdfifo = (DATAFIFO*)mymalloc(SRAMIN,sizeof(DATAFIFO));
	if (pdfifo == NULL)
	{
		printf ("Datafifo memory error!!\r\n");
		return NULL;
	}
	pdfifo->size = set_size;
	pdfifo->front = 0;
	pdfifo->rear = 0;
	
	pdfifo->elem = (u8*)mymalloc(SRAMIN,pdfifo->size);
	if (pdfifo->elem == NULL)
	{
		printf ("Datafifo elem memory error!!\r\n");
		return NULL;
	}
		
	return pdfifo;
}
#else
/*******************************************************************
函数原形： void init_data_fifo (DATAFIFO* pdatafifo)
功能：  	初始化数据fifo相关参数；
参数：     
*******************************************************************/
void init_data_fifo (DATAFIFO* pdatafifo)
{

	pdatafifo->size = 32;
	pdatafifo->front = 0;
	pdatafifo->rear = 0;
}
#endif

/*******************************************************************
函数原形： 	u8 insert_data_fifo (DATAFIFO* pdfifo, u8 elem)
功能：  数据fifo字节插入；
参数：  pdfifo要操作fifo的地址，elem要插入的数据
返回：1表示fifo满
*******************************************************************/
u8 insert_data_fifo (DATAFIFO* pdfifo, u8 elem)
{
	u32 j = 0;

	j = (pdfifo->rear +1 )%pdfifo->size;

	if ( pdfifo->front ==  j) return 1;
	
	pdfifo->elem[pdfifo->rear] = elem;
		
	pdfifo->rear = j;	//插入


	return 0;
}


/*******************************************************************
函数原形： u8 delete_data_fifo(DATAFIFO* pdfifo, u8 *pelem)
功能：  数据fifo包删除；
参数：  pdfifo要操作fifo的地址，pelem从fifo中移除数据转存地址
返回：1表示fifo里面无数据
*******************************************************************/
u8 delete_data_fifo(DATAFIFO* pdfifo, u8 *pelem)
{
	if ( pdfifo->front == pdfifo->rear)return 1;								   //数据fifo空返回错误1
	
	*pelem = pdfifo->elem[pdfifo->front];

	pdfifo->front = (pdfifo->front +1)%pdfifo->size;

	return 0;
}

