/**********************************************************/
//公司名称：保力通讯设备有限公司
//创建时间：2012-10-16
//作者：ke
//文件名：message.c
//功能描述：该文件提供一个先进先出的系统信息缓冲区	
/**********************************************************/
#include "message.h"

typedef struct _msg_fifo
{
 	u8		front;		//数据包读地址
	u8		rear;		//数据包写地址
	u8		elem[MAX_SYS_MSG_FIFO_DEPTH];	  //缓冲区


} msg_fifo;

msg_fifo msg_fifos[SYS_MSG_FIFO_COUNT];


/*******************************************************************
函数原形： void init_message_fifo (u8 msg_fifo_index)
功能：  	初始化相应的信息fifo
参数：     msg_fifo_index需要初始化信息fifo的索引
*******************************************************************/
void init_message_fifo (u8 msg_fifo_index)
{
	msg_fifo* p_msg_fifo = &msg_fifos [msg_fifo_index];

	p_msg_fifo->front = 0;

	p_msg_fifo->rear = 0;
}

/*******************************************************************
函数原形： void message_init (void)
功能：  	初始化信息fifo
*******************************************************************/
void message_init (void)
{
	init_message_fifo (SYS_MSG_FIFO_DEV_CTRL);
	init_message_fifo (SYS_MSG_FIFO_GUI);	
}

/*******************************************************************
函数原形： u8 message_send (u8 msg_fifo_index, u8 event)
功能：  	写入信息到信息fifo
参数：     msg_fifo_index信息fifo索引，event信息
返回：1表示信息fifo已满，0表示成功写入
*******************************************************************/
u8 message_send (u8 msg_fifo_index, u8 event)
{
	msg_fifo* p_msg_fifo = &msg_fifos [msg_fifo_index];
	u8 j = 0;

	j = (p_msg_fifo->rear +1 )%MAX_SYS_MSG_FIFO_DEPTH;

	if ( p_msg_fifo->front ==  j) return 1;

	p_msg_fifo->elem[p_msg_fifo->rear] = event;
		
	p_msg_fifo->rear = j;	//插入

	return 0;
}

/*******************************************************************
函数原形： u8 message_get(u8 msg_fifo_index)
功能：  从信息fifo读出信息
参数：     msg_fifo_index信息fifo索引
返回：信息
*******************************************************************/
u8 message_get(u8 msg_fifo_index)
{
	msg_fifo* p_msg_fifo = &msg_fifos [msg_fifo_index];
	u8  event = MSG_NONE;

	if ( p_msg_fifo->front == p_msg_fifo->rear)return event;								   //数据fifo空返回错误1
	
	event = p_msg_fifo->elem[p_msg_fifo->front];

	p_msg_fifo->front = (p_msg_fifo->front +1)%MAX_SYS_MSG_FIFO_DEPTH;

	return event;
}
