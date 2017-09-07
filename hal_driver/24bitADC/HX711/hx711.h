#ifndef __HX711_H__
#define __HX711_H__			 
#include "main.h"	 	
#include "gpio.h"

#define HX711_SCK   PCout(3)
#define HX711_DATA  PCin(2)

extern u32 hx711_adc[3];
extern int hx711_temp;
extern int hx711_err;

extern void hx711_init(void);
extern int hx711_check (void);
extern void hx711_evet_get (void);

#endif	   


