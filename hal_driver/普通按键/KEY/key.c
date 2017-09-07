/**********************************************************/
//公司名称：
//创建时间：2017-8-28
//作者：ke
//文件名：key.c
//功能描述：该文件为按键驱动文件，需要mytimer.c支持 	
/**********************************************************/
#include "main.h"
#include "gpio.h"
#include "key.h"
#include "mytimer.h"
#include "message.h"

#define KEY1 PAin(5)
#define KEY2 PAin(6)	
#define KEY3 PAin(7)	
#define KEY4 PCin(4)	


#define 	KEY_SCAN_TIME			10			//按键扫描间隔时间
#define 	KEY_JITTER_TIME			20			//按键按下最小时间
#define 	KEY_CP_TIME				1000		//按键长按下最小时间
#define 	KEY_CPH_TIME			500			//按键长按保持最小时间

#define		KEY_COUNT			4 			//按键数量

typedef enum _KEY_STATE
{
	KEY_STATE_IDLE,
	KEY_STATE_JITTER,
	KEY_STATE_PRESS_DOWN,
	KEY_STATE_CP,
} KEY_STATE;


TIMER			key_wait_timer;
TIMER			key_scan_timer;
KEY_STATE	    key_state;


static unsigned char const key_event[KEY_COUNT][4] = 
{
/*  短按          长按开始		    长按保持	      长按结束			*/
/*	SP					  CPS					  CPH				    CPR					*/
	{KEY_MSG_K1,    MSG_NONE, 		MSG_NONE,   	MSG_NONE		},	/*SW0*/
	{KEY_MSG_K2, 	  KEY_MSG_K2, 	KEY_MSG_K2,   MSG_NONE		},	/*SW1*/
	{KEY_MSG_K3,    KEY_MSG_K3, 	KEY_MSG_K3, 	MSG_NONE		},	/*SW2*/
	{KEY_MSG_K4,	  MSG_NONE, 		MSG_NONE,   	MSG_NONE	  },	/*SW3*/
};



/*******************************************************************
函数原形：void key_init (void)
功能：  初始化按键
*******************************************************************/
void key_init (void)
{
	key_state = KEY_STATE_IDLE;
	time_out_set(&key_scan_timer,0);
}

/*******************************************************************
函数原形：u8 get_key_index (void)
功能：  获取按键采样值并转换成索引值
返回： 按键索引值
*******************************************************************/
u8 get_key_index (void)
{
	u8	key_index = 0xff;
	
	if (KEY1 == 0)
	{
		key_index = 0;
	}
	else if (KEY2 == 0)
	{
		key_index = 1;
	}
	else if (KEY3 == 0)
	{
		key_index = 2;
	}
	else if (KEY4 == 0)
	{
		key_index = 3;
	}

	return key_index;
}


/*******************************************************************
函数原形：void key_event_get (void)
功能： 判断按键状态：短按、长按和放开按键 
*******************************************************************/
void key_event_get (void)
{
	static 	u8 	pre_key_index = 0xff;
	u8			key_index;
	u8			event = MSG_NONE;

	if(!is_time_out(&key_scan_timer))
	{
		return ;
	}
	time_out_set(&key_scan_timer, KEY_SCAN_TIME);	

	key_index = get_key_index();

	switch(key_state)
	{
		case KEY_STATE_IDLE:{
			if(key_index == 0xff)
			{
				return;
			}

			pre_key_index = key_index;
			time_out_set(&key_wait_timer, KEY_JITTER_TIME);
		//	printf ("GOTO JITTER!!!\r\n");
			key_state = KEY_STATE_JITTER;
		}
				
		case KEY_STATE_JITTER:{
			if(pre_key_index != key_index)
			{
			//	printf ("GOTO IDLE Because jitter!!!\r\n");
				key_state = KEY_STATE_IDLE;
			}
			else if(is_time_out(&key_wait_timer))
			{
			//	printf ("GOTO PRESS_DOWN!!!\r\n");
 				time_out_set(&key_wait_timer, KEY_CP_TIME);
				key_state = KEY_STATE_PRESS_DOWN;
			}
		}break;

		case KEY_STATE_PRESS_DOWN:{
			if(pre_key_index != key_index)
			{
				//return key sp value
				//printf ("KEY SP 0x%x\r\n", pre_key_index);
				key_state = KEY_STATE_IDLE;
				event = key_event[pre_key_index][0];
			}
			else if(is_time_out(&key_wait_timer))
			{
				//return key cp value
				//printf ("KEY CPS!!!\r\n");
				time_out_set(&key_wait_timer, KEY_CPH_TIME);
				key_state = KEY_STATE_CP;
				event = key_event[pre_key_index][1];
			}
		}break;

		case KEY_STATE_CP:{
			if(pre_key_index != key_index)
			{
				//return key cp value
				//printf ("KEY CPR!!!\r\n");
				key_state = KEY_STATE_IDLE;
				event = key_event[pre_key_index][3];
			}
			else if(is_time_out(&key_wait_timer))
			{
				//return key cph value
				//printf ("KEY CPH 0x%x\r\n", pre_key_index);
				time_out_set(&key_wait_timer, KEY_CPH_TIME);
				event = key_event[pre_key_index][2];
			}
		}break;

		default:{
			key_state = KEY_STATE_IDLE;
		}break;
	}
//	message_send (SYS_MSG_FIFO_DEV_CTRL, event);
	message_send (SYS_MSG_FIFO_GUI, event);
}
