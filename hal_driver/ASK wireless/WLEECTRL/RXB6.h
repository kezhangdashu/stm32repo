#ifndef __RXB6_H__
#define __RXB6_H__			 
#include "main.h"	 	
#include "gpio.h"

#define MESSAGE_SUPPLY           //消息队列支持

//extern u8 rxb6_bit_buf[32];

extern void rxb6_init (void);
extern int rxb6_get (void);

#endif
