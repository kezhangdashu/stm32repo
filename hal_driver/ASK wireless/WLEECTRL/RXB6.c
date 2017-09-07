/**********************************************************/
//公司名称：
//创建时间：2017-8-26
//作者：ke
//文件名：RXB6.c
//功能描述：ASK无线收发器RXB6驱动
//使用1个定时器和1个外部中断，定时器计数单位1us，外部中断开启上下边沿中断
/**********************************************************/
#include <string.h>
#include "RXB6.h"
#include "tim.h"
#include "led.h"
#include "cmd.h"

#ifdef MESSAGE_SUPPLY
#include "message.h"
#endif
 
	
#define RXB6DA PCin(9)

#define ASK_ADRR_CNT 16        //地址位数
#define ASK_KEY_CNT  12        //遥控按键数

u8 rxb6_bit_state = 0;
u16 rxb6_bit_cnt = 0;
u32 rxb6_frame = 0;
u32 rxb6_frame_last = 0;
u32 rxb6_htim = 0;          //高电平时间
u32 rxb6_ltim = 0;          //低电平时间
u32 rxb6_period = 0;        //周期
//u32 rxb6_duty = 0;          //占空比
u8 tim_overfg = 0;

u8 rxb6_bit_buf[32];

const u8 ask_addr[ASK_ADRR_CNT] = {0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};           //地址码

//const u8 ask_coder_1on  [24-ASK_ADRR_CNT] = {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01};
//const u8 ask_coder_1off [24-ASK_ADRR_CNT] = {0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00};

//const u8 ask_coder_2on  [24-ASK_ADRR_CNT] = {0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01};
//const u8 ask_coder_2off [24-ASK_ADRR_CNT] = {0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00};

//const u8 ask_coder_3on  [24-ASK_ADRR_CNT] = {0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01};
//const u8 ask_coder_3off [24-ASK_ADRR_CNT] = {0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00};

//const u8 ask_coder_dm   [24-ASK_ADRR_CNT] = {0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01};
//const u8 ask_coder_ql   [24-ASK_ADRR_CNT] = {0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00};

//const u8 ask_coder_5se  [24-ASK_ADRR_CNT] = {0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01};
//const u8 ask_coder_15se [24-ASK_ADRR_CNT] = {0x01,0x01,0x00,0x00,0x01,0x01,0x00,0x00};
//const u8 ask_coder_30se [24-ASK_ADRR_CNT] = {0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01};
//const u8 ask_coder_stop [24-ASK_ADRR_CNT] = {0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00};

const u8 ask_coder[ASK_KEY_CNT][24-ASK_ADRR_CNT]={
	{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01},             //1开
	{0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00},             //1关
	
	{0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01},             //2开
	{0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00},             //2关
	
	{0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01},             //3开
	{0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00},             //3关
	
	{0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01},             //对码
	{0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00},             //清料
	
	{0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01},             //5秒
	{0x01,0x01,0x00,0x00,0x01,0x01,0x00,0x00},             //15秒
	{0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01},             //30秒
	{0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00},             //停止
};

const char *ask_key_name[ASK_KEY_CNT] = {
	"1 ON  ",
	"1 OFF ",
	"2 ON  ",
	"2 OFF ",
	"3 ON  ",
	"3 OFF ",
	"CODER ",
	"CLEAN ",
	"5  SEC",
	"15 SEC",
	"30 SEC",
	"STOP  ",
};

#ifdef MESSAGE_SUPPLY
const SYS_MESSAGE ask_key_msg[ASK_KEY_CNT] = {
	WKY_MSG_1ON,
	WKY_MSG_1OFF,
	WKY_MSG_2ON,
	WKY_MSG_2OFF,
	WKY_MSG_3ON,
	WKY_MSG_3OFF,
	WKY_MSG_CODER,
	WKY_MSG_CLEAN,
	WKY_MSG_5SEC,
	WKY_MSG_15SEC,
	WKY_MSG_30SEC,
	WKY_MSG_STOP,
};
#endif

	
void TIM4_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET)
  {
    if(__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_UPDATE) !=RESET)
    {
      __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
			HAL_TIM_Base_Stop_IT(&htim4);
			htim4.Instance->CNT = 0;         //清零计数值
			tim_overfg = 1;           //定时器溢出
    }
  }
}


//void EXTI9_5_IRQHandler(void)
//{
//	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET) 
//  { 
//    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
//		
//		if (tim_overfg)
//		{
//			rxb6_bit_state = 0;
//			tim_overfg = 0;
//		}
//		
//		if (RXB6DA == 1)     //上升沿
//		{
//			rxb6_period = htim4.Instance->CNT;
//			if ((rxb6_bit_state == 2) && (rxb6_period > 0))
//			{
//				rxb6_ltim = rxb6_period - rxb6_htim;
//			  rxb6_duty = (rxb6_htim*100)/rxb6_period;   //得到占空比
//			}
//			HAL_TIM_Base_Stop_IT(&htim4);
//			htim4.Instance->CNT = 0;
//			HAL_TIM_Base_Start_IT(&htim4);
//			rxb6_bit_state = 1;
//			putchar (0xaa);
//			putchar (rxb6_htim>>8);
//			putchar (rxb6_htim);
//			putchar (rxb6_ltim>>8);
//			putchar (rxb6_ltim);
//		}
//		else                 //下降沿
//		{
//			if (rxb6_bit_state == 1)
//			{
//				rxb6_htim = htim4.Instance->CNT;
//				rxb6_bit_state++;
//			}
//		}
//  }
//}

void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET) 
  { 
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
		
		if (tim_overfg)
		{
			rxb6_bit_state = 0;
			tim_overfg = 0;
			rxb6_bit_cnt = 0;
		}
		
		if (RXB6DA == 1)     //上升沿
		{
			rxb6_period = htim4.Instance->CNT;
			if ((rxb6_bit_state == 2) && (rxb6_period > 0))
			{
				rxb6_ltim = rxb6_period - rxb6_htim;
//			  rxb6_duty = (rxb6_htim*100)/rxb6_period;   //得到占空比

				if ((rxb6_ltim >= 3000) && (rxb6_ltim <= 6000) && (rxb6_htim >= 130) && (rxb6_htim <= 190))      //检测起始位
				{
			//		if ((rxb6_duty >= 2) && (rxb6_duty <= 5))
			//		{
						rxb6_bit_cnt = 1;
			//		  putchar (0xaa);
			//		}
				}
				else if (rxb6_bit_cnt > 0)
				{
					if (rxb6_htim > rxb6_ltim)  //检测1
					{
					//	putchar (0x01);
						rxb6_bit_buf[rxb6_bit_cnt-1] = 1;
						if (rxb6_bit_cnt == 24)
						{
							rxb6_frame++;
							rxb6_bit_cnt = 0;
						}
						else rxb6_bit_cnt++;
					}
					else                     //检测0
					{
					//	putchar (0x00);
						rxb6_bit_buf[rxb6_bit_cnt-1] = 0;
						if (rxb6_bit_cnt == 24)
						{
							rxb6_frame++;
							rxb6_bit_cnt = 0;
						}
						else rxb6_bit_cnt++;
					}		
				}
			}
			
			HAL_TIM_Base_Stop_IT(&htim4);
			htim4.Instance->CNT = 0;
			HAL_TIM_Base_Start_IT(&htim4);
			rxb6_bit_state = 1;
		}
		else                 //下降沿
		{
			if (rxb6_bit_state == 1)
			{
				rxb6_htim = htim4.Instance->CNT;
				rxb6_bit_state++;
			}
		}
  }
}

/*******************************************************************
函数原形： void rxb6_init (void)
功能：  	初始化
参数：   无
返回：   无
*******************************************************************/
void rxb6_init (void)
{
	rxb6_bit_state = 0;
	rxb6_bit_cnt = 0;
	rxb6_frame = 0;
	rxb6_frame_last = 0;
	rxb6_htim = 0;          //高电平时间
	rxb6_ltim = 0;          //低电平时间
	rxb6_period = 0;        //周期
//	rxb6_duty = 0;          //占空比
	tim_overfg = 0;
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
//	HAL_TIM_Base_Start_IT(&htim4);
}

/*******************************************************************
函数原形： int rxb6_get (void)
功能：  	获取无线按键值
参数：   无
返回：   0成功，其他值失败
*******************************************************************/
int rxb6_get (void)
{
	u32 i;
	
	if (rxb6_frame != rxb6_frame_last)
	{
		rxb6_frame_last = rxb6_frame;
		if (memcmp(rxb6_bit_buf,ask_addr,ASK_ADRR_CNT) == 0)
		{
		//	cmd_putascii (rxb6_bit_buf+ASK_ADRR_CNT,24-ASK_ADRR_CNT);
			for (i = 0; i < ASK_KEY_CNT; i++)
			{
				if (memcmp (rxb6_bit_buf+ASK_ADRR_CNT,&ask_coder[i][0],24-ASK_ADRR_CNT) == 0)break;
			}
			
			if (i < ASK_KEY_CNT)
			{
#ifdef MESSAGE_SUPPLY
				message_send (SYS_MSG_FIFO_DEV_CTRL,ask_key_msg[i]);
//				message_send (SYS_MSG_FIFO_GUI,ask_key_msg[i]);
#endif
			//	printf ("wkey %s\r\n",ask_key_name[i]);
			}
//			else
//			{
//				printf ("unkown wkey: ");
//				cmd_putascii (rxb6_bit_buf+ASK_ADRR_CNT,24-ASK_ADRR_CNT);
//			}
		}
		return 0;
	}
	return -1;
}
