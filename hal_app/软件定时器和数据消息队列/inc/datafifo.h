#ifndef __DATAFIFO_H__
#define __DATAFIFO_H__

#include "main.h"

typedef __packed struct _DATAFIFO
{
	u32     size;       /*缓冲区大小*/
 	u32		front;		/*读地址*/
	u32		rear;		/*写地址*/
	#ifdef SUPDYMEM
	u8		*elem;	    /*缓冲区首地址*/
	#else
	u8    elem[32];
	#endif
} DATAFIFO;

#ifdef SUPDYMEM
extern DATAFIFO* init_data_fifo (u32 set_size);
#else
extern void init_data_fifo (DATAFIFO* pdatafifo);
#endif
extern u8 insert_data_fifo (DATAFIFO* pdfifo, u8 elem);
extern u8 delete_data_fifo(DATAFIFO* pdfifo, u8 *pelem);

#endif
