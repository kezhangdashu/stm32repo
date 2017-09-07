#ifndef __DMX512_H
#define __DMX512_H			 
#include "main.h"	 	
#include "gpio.h"


extern u8 dmx_lost;        
extern u8 DMX_ADC[3];

extern void DMX_init(void);
extern void DMX_get (u8 dmxc);


#endif	   

