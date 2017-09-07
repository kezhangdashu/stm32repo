/**********************************************************/
//公司名称：
//创建时间：2012-10-16
//作者：ke
//文件名：mytimer.c
//功能描述：该文件提供了单位为ms的标准定时和延时函数，需要timer.c的支持 	
/**********************************************************/
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "mytimer.h"

//u32 gmy_sys_tick;		   /*系统全局计时器*/

/*******************************************************************
函数原形：void sys_tick_init (void)
功能：    初始化系统时钟
*******************************************************************/
//void sys_tick_init (void)
//{
//	gmy_sys_tick = 0;
//	TIM3_Int_Init(10-1,7200-1);	/*初始化定时器3作为系统全局时钟，定时1ms*/	
//}

/*******************************************************************
函数原形：void timer_tick (void)
功能：   系统时钟自加
*******************************************************************/
//void timer_tick (void)
//{
//	gmy_sys_tick++;	  /*软件系统主时钟，1ms自加1*/
//}

/*******************************************************************
函数原形：u32 time_now (void)
功能：   返回系统时钟
返回： 系统时钟计数值
*******************************************************************/
//u32 time_now (void)
//{
//	return gmy_sys_tick; /*返回软件系统主时钟单位ms*/
//}

/*******************************************************************
函数原形：u32 past_time_get (TIMER* ptimer)
功能：   返回指定定时器超过的时间
参数：	 定时器的地址
返回：   超过的时间单位ms
*******************************************************************/
u32 past_time_get (TIMER* ptimer)
{
	u32 time_now_temp = HAL_GetTick ();
	time_now_temp -= ptimer->rec_tick_val;
	return time_now_temp;	/*返回软件时间超过时间单位ms*/
}

/*******************************************************************
函数原形：void time_out_set (TIMER* stimer, u32 timeout)
功能：   设置指定定时器定时时间
参数：	 timer定时器的地址，timeout定时时间
*******************************************************************/
void time_out_set (TIMER* stimer, u32 timeout)
{
	if (stimer == 0)
	{
		printf ("stimer memory error!!\r\n");
		return;
	}
	stimer->time_out_val = timeout;				  /*初始化TIMER结构体所有成员*/
	stimer->rec_tick_val = HAL_GetTick ();
	stimer->is_time_out = 0;
}

/*******************************************************************
函数原形：u8 is_time_out (TIMER* otimer)
功能：   查询指定定时器是否超时
参数：	 timer定时器的地址
返回：   超时标志 1表示超时，否则没有超时
*******************************************************************/
u8 is_time_out (TIMER* otimer)
{
	u32 time_now_temp;
	
    if (otimer == 0)
	{
		printf ("otimer memory error!!\r\n");
		return 0;
	}
	
	time_now_temp = past_time_get (otimer);

	if (time_now_temp < (otimer->time_out_val))
	{
		otimer->is_time_out = 0;
		
	}
	else
	{
		otimer->is_time_out = 1;
	}
	
	return otimer->is_time_out;

}

/*******************************************************************
函数原形：void delay_se (u32 sec)
功能：   秒延时
参数：	 sec延时时间，sec<=4294967
*******************************************************************/
TIMER tsec;
void delay_se (u32 sec)
{
	time_out_set (&tsec, sec*1000);
	
	while (is_time_out (&tsec) == 0);
}
