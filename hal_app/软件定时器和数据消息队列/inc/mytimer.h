#ifndef __MYTIMER_H__
#define __MYTIMER_H__

#include "main.h"

/* define softtimer structure. */
typedef struct	_TIMER
{
	u32	time_out_val; 								/*time out value*/
	u32	rec_tick_val;			  						/*softtimer setting value*/
	u8	is_time_out;									/*time out flag*/
} TIMER;


extern u32 gmy_sys_tick; 					/*system tick counter*/


/* System tick (1ms) initilization.*/
//extern void sys_tick_init (void);
		
//extern void timer_tick (void);

/* Get now time from system start.*/
//extern u32 time_now (void);

/* Get time of some softtimer from setting to now.*/
extern u32 past_time_get (TIMER* ptimer);

// Set time out value of softtimer.
extern void time_out_set (TIMER* stimer, u32 timeout);

// Check whether time out.
extern u8 is_time_out (TIMER* otimer);

extern void delay_se (u32 sec);

#endif
