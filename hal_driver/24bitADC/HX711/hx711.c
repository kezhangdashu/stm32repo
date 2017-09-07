/**********************************************************/
//公司名称：
//创建时间：2017-8-25
//作者：ke
//文件名：hx711.c
//功能描述：HX711驱动
/**********************************************************/
#include <string.h>	
#include "cmd.h"
#include "hx711.h"	 
#include "delay.h"
#include "mytimer.h"

#define ADC_SCAN_TIM 200         //adc扫描间隔时间单位ms


u32 hx711_adc[3] = {0,0,0};
int hx711_err = 0;
u32 hx711_err_cnt = 0;
int hx711_temp = 0;
TIMER tscan_adc;


/*******************************************************************
函数原形： u32 hx711_adc_get (void)
功能：ADC单次采样
参数：无
返回：adc值
*******************************************************************/
u32 hx711_adc_get (void)
{
	u32 temp = 0,i = 0;
	HX711_SCK = 0;
	
	while (HX711_DATA)
	{
		delay_ms(1);
		if ((++i) > 1000)
		{
			DEBUG ("HX711 ERR\r\n");
			return 0;
		}
	}
	
	for (i = 0; i < 24; i++)
	{
		HX711_SCK = 1;
		delay_us (1);
		temp = temp<<1;
		HX711_SCK = 0;
		if (HX711_DATA)temp++;
		delay_us (1);
	}
	HX711_SCK = 1;
	delay_us (1);
//	temp = temp&0x00ffffff;
	temp = temp^0x800000;
	HX711_SCK = 0;
	delay_us (1);
	
	HX711_SCK = 1;
	delay_us (1);
	HX711_SCK = 0;
	delay_us (1);
	
	return temp;
}

/*******************************************************************
函数原形： void hx711_init(void)
功能：ADC初始化
参数：无
返回：无
*******************************************************************/
void hx711_init(void)
{
	HX711_SCK = 0;
	hx711_err = 0;
	time_out_set (&tscan_adc,0);
}

/*******************************************************************
函数原形： void hx711_temp_get (void)
功能：获取温度
参数：无
返回：无
*******************************************************************/
void hx711_temp_get (void)
{
	u32 k;
	
	k = hx711_adc_get ();
	if (k != 0)
	{
		hx711_adc[0] = k;
		hx711_adc[1] = hx711_adc[0]/2542;
		hx711_temp = (hx711_adc[1]/2)+30-1649;
		hx711_err_cnt = 0;
	}
	else
	{
		if ((++hx711_err_cnt) > 20)
		{
			hx711_err = -1;
		}
	}
}

/*******************************************************************
函数原形： int hx711_check (void)
功能：检查hx711
参数：无
返回：0正常，其他值有问题
*******************************************************************/
int hx711_check (void)
{
	u8 k = 0;
	for (k = 0; k < 10; k++)
	{
		hx711_temp_get ();
	}
	return hx711_err;
}

/*******************************************************************
函数原形： void hx711_init(void)
功能：ADC初始化
参数：无
返回：无
*******************************************************************/
void hx711_evet_get (void)
{
	if (is_time_out (&tscan_adc))
	{
		hx711_temp_get ();
		time_out_set (&tscan_adc,ADC_SCAN_TIM);
	}
}



