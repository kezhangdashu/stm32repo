/**********************************************************/
//公司名称：
//创建时间：2017-7-13
//作者：ke
//文件名：esp8266_api.c
//功能描述：esp8266api实现	
/**********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp8266_io.h"
#include "esp8266_api.h"
#include "delay.h"
#include "mytimer.h"
#include "malloc.h"
#include "cmd.h"

WIFI_MOD *gwifi_mod;
u32 repread_cnt = 0;                      //防止某些命令返回含有TCP接收数据

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ESP8266_CWMODE_TBL[3]={"STA mode","AP mode","AP&STAmode"};	//ATK-ESP8266,3种网络模式
//4种连接状态
const u8 *ESP8266_CIPSTATUS[6] = {"unknow","unknow","get ip","connect","disconnect","wifi lost"};
//4种工作模式
//const u8 *ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
//const u8 *ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
/*******************************************************************
函数原形： const char *my_strstr(const char *s1, const char *s2, int size)
功能：比较字符串，从s1中找出与s2相同的,size为s1的大小
参数：s1待比较的数组，s2要比较的字符串，size为s1的大小
返回值:返回首地址，没有找到返回NULL
*******************************************************************/
//const char *my_strstr(const char *s1, const char *s2,int size)
//{
//	int len = strlen(s2);
//	int i=0,sta=0;
//	const char *strx = NULL;
//	for (i = 0;i < size; i++)
//	{
//		if (s1[i] == s2[sta])
//		{
//			if (sta == 0)strx = s1+i;
//			sta++;
//			if (sta == len)break;
//		}
//		else if ((sta != 1) || (s1[i] != s2[0]))
//		{
//			sta = 0;
//		}
//	}
//	
//	if (sta != len)strx = NULL;
//	
//	return strx;
//	
//}

/*******************************************************************
函数原形： u8* esp8266_check_cmdno(u8 *str,u32 len)
功能：    ESP8266发送命令后,检测接收到的应答
参数：str:期待的应答结果
返回值:0,没有得到期待的应答结果,其他,期待应答结果的位置(str的位置)
*******************************************************************/
//u8* esp8266_check_cmdno(u8 *str,u32 len)
//{
//	
//	const char *strx=0;
// 
////	uart3_rxbuf[uart3_rx_cnt]=0;          //添加结束符
////	strx=strstr((const char*)uart3_rxbuf,(const char*)str);
//	strx=my_strstr((const char*)uart3_rxbuf,(const char*)str,len);
//	
//	return (u8*)strx;
//}

/*******************************************************************
函数原形： u8* esp8266_check_cmd(u8 *str)
功能：    ESP8266发送命令后,检测接收到的应答
参数：str:期待的应答结果
返回值:0,没有得到期待的应答结果,其他,期待应答结果的位置(str的位置)
*******************************************************************/
u8* esp8266_check_cmd(u8 *str)
{
	
	char *strx=0;
 
//	uart3_rxbuf[uart3_rx_cnt]=0;          //添加结束符
	strx=strstr((const char*)uart3_rxbuf,(const char*)str);

	return (u8*)strx;
}


/*******************************************************************
函数原形： u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
功能：    向ESP8266发送命令
参数：cmd:发送的命令字符串 ack:期待的应答结果,如果为空,则表示不需要等待应答 waittime:等待时间(单位:10ms)
返回值:0,发送成功(得到了期待的应答结果)1,发送失败
*******************************************************************/
u8 esp8266_debug = 0;
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{ 
	u8 res = 1;
	u32 len = 0;
	if (esp8266_debug)
	{
		printf ("%s\r\n",cmd);
	}
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack && waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			len = uart3_get_data ();
			if(len)//接收到期待的应答结果
			{
				if (esp8266_debug)
				{
					printf ("get %u Byte\r\n",len);
					cmd_putmore (uart3_rxbuf,len);
				}
				if(esp8266_check_cmd(ack))
				{
				//	printf("ack:%s\r\n",(u8*)ack);
					res = 0;
					break;//得到有效数据 
				}
			} 
		} 
	}
	return res;
}

/*******************************************************************
函数原形： u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
功能：    向ESP8266发送指定数据
参数：data:发送的数据(不需要添加回车了) ack:期待的应答结果,如果为空,则表示不需要等待应答 waittime:等待时间(单位:10ms)
返回值:0,发送成功(得到了期待的应答结果)1,发送失败
*******************************************************************/
//u8 esp8266_send_data(u8 *data,u8 *ack,u16 waittime)
//{
//	u8 res = 1;
//	u32 len = 0;
//	u3_printf("%s",data);	//发送命令
//	if(ack && waittime)		//需要等待应答
//	{
//		while(--waittime)	//等待倒计时
//		{
//			delay_ms(10);
//			len = uart3_get_data ();
//			if(len)//接收到期待的应答结果
//			{
//				if(esp8266_check_cmd(ack))
//				{
//					res = 0;
//					break;//得到有效数据 
//				}
//			} 
//		} 
//	}
//	return res;
//}

/*******************************************************************
函数原形： u8 esp8266_quit_trans(void)
功能：    ESP8266退出透传模式
参数：无
返回值:0,退出成功 1,退出失败
*******************************************************************/
u8 esp8266_quit_trans(void)
{
//	while((USART3->SR&0X40)==0);	//等待发送空
//	USART3->DR='+';      
//	delay_ms(15);					//大于串口组帧时间(10ms)
//	while((USART3->SR&0X40)==0);	//等待发送空
//	USART3->DR='+';      
//	delay_ms(15);					//大于串口组帧时间(10ms)
//	while((USART3->SR&0X40)==0);	//等待发送空
//	USART3->DR='+';    
	u3_putchar ('+');
	u3_putchar ('+');
	u3_putchar ('+');
	delay_ms(500);					//等待500ms
	return esp8266_send_cmd("AT","OK",30);//退出透传判断.
}

/*******************************************************************
函数原形： u8 esp8266_apsta_check(void)
功能：    获取ESP8266模块的AP+STA连接状态
参数：无
返回值:0，未连接;1,连接成功
*******************************************************************/
u8 esp8266_apsta_check(void)
{
	esp8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	if(esp8266_check_cmd("+CIPSTATUS:0")&&
	   esp8266_check_cmd("+CIPSTATUS:1")&&
	   esp8266_check_cmd("+CIPSTATUS:2")&&
	   esp8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}

/*******************************************************************
函数原形： u8 esp8266_consta_check(void)
功能：    获取ESP8266模块的连接状态
参数：无
返回值:连接类型
*******************************************************************/
u8 esp8266_consta_check(void)
{
	u8 *p = 0;
	u8 res = '0';

//	esp8266_debug = 1;
	if (esp8266_send_cmd("AT+CIPSTATUS","STATUS:",50) == 0)	//发送AT+CIPSTATUS指令,查询连接状态
	{
		p=esp8266_check_cmd(":"); 
		if (p != 0)res=*(p+1);									//得到连接状态
	}
	else
	{
		printf ("no find CPISTATUS\r\n");
	}
//	esp8266_debug = 0;
	return res;
}

/*******************************************************************
函数原形： void esp8266_get_wanip(void)
功能：   获取Client ip地址
参数：无
返回值:0成功，1失败
*******************************************************************/
u8 esp8266_get_wanip(void)
{
	u8 *p,*p1;
	if(esp8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
	{
		return 1;
	}		
	p=esp8266_check_cmd("\"");
	p1=(u8*)strstr((const char*)(p+1),"\"");
	*p1=0;
	sprintf((char*)gwifi_mod->ip,"%s",p+1);	
	return 0;
}

/*******************************************************************
函数原形： void esp8266_get_mac(void)
功能：   获取mac地址
参数：无
返回值:0成功，1失败
*******************************************************************/
u8 esp8266_get_mac(void)
{
	u8 *p = 0,*p1 = 0;
	if(esp8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
	{
		return 1;
	}		
	p=esp8266_check_cmd("\"");
	p=(u8*)strstr((const char*)p+1,"\"");
	p=(u8*)strstr((const char*)p+1,"\"");
	p1=(u8*)strstr((const char*)p+1,"\"");
	*p1=0;
	sprintf((char*)gwifi_mod->mac,"%s",p+1);
	return 0;
}


/*******************************************************************
函数原形： u8 esp8266_parse_line (u8 *lines, u8 *argv[],u8 cmdcnt)
功能：分析返回参数
参数：lines要分析的数据，argv各个参数存储首地址，cmdcnt要分析的参数个数
返回值:得到的参数个数
*******************************************************************/
u8 esp8266_parse_line (u8 *lines, u8 *argv[],u8 cmdcnt)
{
	u8 nargs = 0;

	while (nargs < cmdcnt) 
	{

		/* skip any white space */
		while ((*lines == ' ') || (*lines == '\t')||(*lines == '\"')) {
			++lines;
		}

		if (*lines == 0) 
		{	/* end of line, no more args	*/
			return nargs;
		}

		argv[nargs++] = lines;	/* begin of argument string	*/

		/* find end of string */
		while (*lines && (*lines != ',') && (*lines != '\r') && (*lines != '\n')) {
			++lines;
		}
		if (*lines == 0)
		{
			return nargs;
		}
		else if (*lines == ',')
		{
			if (*(lines-1) == '\"')
			{
				*(lines-1) = 0;
			}
		}
		else if ((*lines == '\r') || (*lines == '\n'))
		{
			*lines = 0;
			return nargs;
		}

		*lines++ = 0;		/* terminate current arg	 */
	}

	return nargs;
}

/*******************************************************************
函数原形： u8 esp8266_get_curjap (void)
功能：获取当前加入AP的参数
参数：无
返回值:0成功，其他值失败
*******************************************************************/
u8 esp8266_get_curjap (void)
{
	u8 *p = 0,*argv[4],res;
	if(esp8266_send_cmd("AT+CWJAP?","OK",500))//获取当前连接AP的信息
	{
		return 1;
	}
	p=esp8266_check_cmd("\"");
	if (p)
	{
		res = esp8266_parse_line (p,argv,4);
		if (res == 4)
		{
			strcpy ((char*)gwifi_mod->jap_ssid,(char*)argv[0]);
			strcpy ((char*)gwifi_mod->jap_mac,(char*)argv[1]);
			strcpy ((char*)gwifi_mod->jap_ch,(char*)argv[2]);
			strcpy ((char*)gwifi_mod->jap_rssi,(char*)argv[3]);
			return 0;
		}
	}
	return 2;
}

/*******************************************************************
函数原形： u8 esp8266_get_gmr (u8 *pgmr)
功能：获取esp8266的固件信息
参数：pgmr要存储的地址
返回值:0成功，其他值失败
*******************************************************************/
u8 esp8266_get_gmr (u8 *pgmr)
{
	u8 *p = 0,res = 1;
	if(esp8266_send_cmd("AT+GMR","OK",30))//获取当前连接AP的信息
	{
		return 1;
	}
	p=esp8266_check_cmd("OK");
	if (p)
	{
		*(p-1) = 0;
		*(p-2) = 0;
		strcpy ((char *)pgmr,(char *)uart3_rxbuf);
		res = 0;
	}
	return res;
}

/*******************************************************************
函数原形： u8 esp8266_join_ap (u8* ssidbuf, u8* passbuf)
功能：加入指定ap
参数：ssidbuf为AP的ssid,passbuf为ap密码
返回值:0成功，其他值失败
*******************************************************************/
u8 esp8266_join_ap (u8* ssidbuf, u8* passbuf)
{
	u8 res = 1;
	u32 waittime = 1500,len = 0;
	
	u3_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",(char*)ssidbuf,(char*)passbuf);	//发送命令

	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd("OK"))
			{
				res = 0;
				break;//得到有效数据 
			}
			else if (esp8266_check_cmd("FAIL"))
			{
				res = 2;
				break;//得到有效数据 
			}
		} 
	} 
	return res;
}

/*******************************************************************
函数原形： u8 esp8266_tcp_connect (u8 *ipbuf, u8 *portbuf)
功能：最为客户端连接制定服务器
参数：ipbuf为服务器IP地址,portbuf为服务器端口号
返回值:0成功，其他值失败
*******************************************************************/
//u8 esp8266_permode = 0;               //穿透模式指示
u8 esp8266_tcp_connect (u8 *ipbuf, u8 *portbuf)
{
	u8 res = 1;
	u32 waittime = 1000,len = 0;
	
	u3_printf("AT+CIPSTART=\"TCP\",\"%s\",%s,1000\r\n",(char*)ipbuf,(char*)portbuf);	//发送命令

	while(--waittime)	//等待倒计时
	{
//		delay_ms(10);
		system_delay (10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd("OK"))
			{
				res = 0;
				break;//得到有效数据 
			}
			else if (esp8266_check_cmd("ERROR"))
			{
				res = 2;
				break;//得到有效数据 
			}
		} 
	} 

//	if (esp8266_permode == 0)
//	{
//		if (esp8266_send_cmd("AT+CIPMODE=1","OK",200) == 0)  //打开透传模式	
//		{
//			esp8266_permode = 1;
//		}
//	}
	
	return res;
}


/*******************************************************************
函数原形： u8 esp8266_tcp_close (void)
功能：关闭TCP连接
参数：无
返回值:0成功，其他值失败
*******************************************************************/
u8 esp8266_tcp_close (void)
{
	u8 res = 1;
	u32 waittime = 30,len = 0;
	
	u3_printf("AT+CIPCLOSE\r\n");	//发送命令

	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd("OK"))
			{
				res = 0;
//				uart3_rx_endma ();
				break;//得到有效数据 
			}
			else if (esp8266_check_cmd("ERROR"))
			{
				res = 2;
//				uart3_rx_endma ();
				break;//得到有效数据 
			}
//			uart3_rx_endma ();
		} 
	} 

	return res;
}

/*******************************************************************
函数原形： u8 esp8266_set_send (void) 
功能：设置TCP发送穿透
参数：无
返回值:0成功，其他值失败
*******************************************************************/
u8 esp8266_set_send (void) 
{
	u8 res = 1;
	u32 waittime = 20,len = 0;
//	u32 i;
	
	
	u3_printf("AT+CIPSEND\r\n");	//发送命令

	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd("OK"))
			{
				res = 0;
//				uart3_rx_endma ();
				break;//得到有效数据 
			}
			else if (esp8266_check_cmd("ERROR"))
			{
				res = 2;
//				uart3_rx_endma ();
				break;//得到有效数据 
			}
//			uart3_rx_endma ();
		} 
	} 
	
	
	return res;
}

/*******************************************************************
函数原形： u8 esp8266_tcp_send (u8 *sendbuf, u32 slen) 
功能：TCP发送数据
参数：sendbuf要发送的数据，slen发送数据长度
返回值:0成功，其他值失败
*******************************************************************/
u8 esp8266_tcp_send (u8 *sendbuf, u32 slen) 
{
	u8 res = 1;
	u32 len = 0;
	u32 i;
	TIMER twait;
	
	if (slen >= UART3_TX_DEEP-48)return 3;   //数据超长
	
//	delay_ms (20);
//	for (i = 0; i < slen; i++)
//	{
//		u3_putchar (sendbuf[i]);
//	}
//	
//	return 0;
	u3_printf("AT+CIPSEND=%u\r\n",slen);	//发送命令
	time_out_set (&twait,2000);
	while(is_time_out (&twait) == 0)	//等待倒计时
	{
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd(">"))
			{
				res = 0;
				break;//得到有效数据 
			}
			else if (esp8266_check_cmd("ERROR"))
			{
				res = 2;
				break;//得到有效数据 
			}
		} 
	} 
	
	if (res == 0)
	{
		for (i = 0; i < slen; i++)
		{
			u3_putchar (sendbuf[i]);
		}
		res = 1;
		len = 0;
		time_out_set (&twait,3000);
		while(is_time_out (&twait) == 0)	//等待倒计时
		{
			len = uart3_get_data ();
			if(len)//接收到期待的应答结果
			{
				repread_cnt = len;
				if(esp8266_check_cmd("SEND OK"))
				{
					res = 0;
					break;//得到有效数据 
				}
				else if (esp8266_check_cmd("SEND FAIL"))
				{
					res = 2;
					break;//得到有效数据 
				}			
//				printf ("tcp send get %u Byte\r\n",len);
//				cmd_putmore (uart3_rxbuf,len);
			}
		}
	}
	//安信可固件bug，在特殊情况下，必须发送一个指令才能把ESP8266串口缓存清空
	if (res == 1)
	{
		for (i = 0; i < 3; i++)
		{
			printf ("send over\r\n");
			u3_printf("AT\r\n");	//发送命令
			time_out_set (&twait,2000);
			while(is_time_out (&twait) == 0)	//等待倒计时
			{
				len = uart3_get_data ();
				if(len)//接收到期待的应答结果
				{
					repread_cnt = len;
					if(esp8266_check_cmd("SEND OK"))
					{
						res = 0;
						break;//得到有效数据 
					}
					else if (esp8266_check_cmd("SEND FAIL"))
					{
						res = 2;
						break;//得到有效数据 
					}			
//				printf ("tcp send get %u Byte\r\n",len);
//				cmd_putmore (uart3_rxbuf,len);
				}
			}
			if (res != 1)break;
		}
	}
	
	if (res == 1)
	{
		printf ("go to debug mode\r\n");
		esp8266_cmd_mode = 1;             //发生错误进入调试模式
	}
	
	return res;
}


/*******************************************************************
函数原形： u32 esp8266_tcp_recv_nowait (u8 *rxbuf) 
功能：TCP数据接收,不等待超时
参数：rxbuf接收的数据
返回值:接收到数据长度
*******************************************************************/
u32 esp8266_tcp_recv_nowait (u8 *rxbuf) 
{
	u32 len = 0,slen = 0;
	u8 *p = NULL,*p0 = NULL;

	if (repread_cnt)             //防止发送其他命令时漏掉TCP接收的数据
	{
		repread_cnt = 0;
		p = esp8266_check_cmd("+IPD");
		if(p != NULL)
		{
//			printf ("re rx TCP\r\n");
			p = (u8*)strstr ((char*)p,",");
			p0 = (u8*)strstr ((char*)p,":");
			*p0 = 0;
			slen = strtol ((char*)p+1, NULL, 10);
//			printf ("TCP get %u byte\r\n",slen);
			memcpy (rxbuf,p0+1,slen);
			return slen;
		}
	}
	
	len = uart3_get_data ();
	if(len)//接收到期待的应答结果
	{
//		printf ("u3 get %u Byte\r\n",len);
//		cmd_putmore (uart3_rxbuf,len);
		p = esp8266_check_cmd("+IPD");
		if(p != NULL)
		{
			p = (u8*)strstr ((char*)p,",");
			p0 = (u8*)strstr ((char*)p,":");
			*p0 = 0;
			slen = strtol ((char*)p+1, NULL, 10);
//			printf ("TCP get %u byte\r\n",slen);
			memcpy (rxbuf,p0+1,slen);
			return slen;
		}
	}
	return 0;
}

/*******************************************************************
函数原形： u8 esp8266_smart_config (u32 timeover,u8* ssidbuf,u8* passbuf)
功能：进行smartconfig操作
参数：timeover超时时间单位10ms，ssidbuf获取的ssid，passbuf获取到的密码
返回值:0完成配置，
*******************************************************************/
u8 esp8266_smart_config (u32 timeover,u8* ssidbuf,u8* passbuf)
{	
	u8 *p = NULL;
	u8 *p1 = NULL,res = 0,*pbuf = NULL;
	u32 waittime = timeover,len = 0;
	
	pbuf = mymalloc(SRAMIN,2048);
	
	if (pbuf == NULL)
	{
		printf ("pbuf memory error!!\r\n");
		while (1);
	}
	
	if(esp8266_send_cmd("AT+CWSTOPSMART","OK",30))    //先释放smartconfig
	{
		myfree (SRAMIN,pbuf);
		return 1;
	}
	
	u3_printf("AT+CWSTARTSMART\r\n");	//发送命令
	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd("ssid:"))
			{
				memcpy (pbuf,uart3_rxbuf,len);
				pbuf [len] = 0;
				res |= 0x01;
				p = (u8*)strstr((const char*)pbuf,"ssid:");
				p += 5;
				p1 = (u8*)strstr((const char*)p,"\r\n");
				*p1 = 0;
				strcpy ((char*)ssidbuf,(char*)p);
			}
			if (esp8266_check_cmd("password:"))
			{
				memcpy (pbuf,uart3_rxbuf,len);
				pbuf [len] = 0;
				res |= 0x02;
				p = (u8*)strstr((const char*)pbuf,"password:");
				p += 9;
				p1 = (u8*)strstr((const char*)p,"\r\n");
				*p1 = 0;
				strcpy ((char*)passbuf,(char*)p);
			}
			if (esp8266_check_cmd ("smartconfig connected wifi"))
			{
				res |= 0x04;
//				uart3_rx_endma ();
				break;
			}
//			uart3_rx_endma ();
		} 
	}
	
	esp8266_send_cmd("AT+CWSTOPSMART","OK",50);   //释放掉smartconfig
	myfree (SRAMIN,pbuf);
	if(res != 0x07)                 //没有完成smartconfig
	{
		if (res == 0x03)            //收到完整AP信息
		{
			printf ("smart config get ap info\r\n");
			if (esp8266_join_ap (ssidbuf,passbuf))
			{
				printf ("join ap fail\r\n");
				return 1;
			}
		}
		else 
		{
			printf ("smart config over time\r\n");
			return 1;
		}
	}
	printf ("join ap ok\r\n");
	printf ("smartconfig ok\r\n");
	return 0;
}

/*******************************************************************
函数原形： void esp8266_clear_rxbuf (u32 timerover)
功能：清除串口接收缓冲区
参数：timerover清除时间单位10ms
返回值:无
*******************************************************************/
void esp8266_clear_rxbuf (u32 timerover)
{
	u32 len = 0,waittime = timerover;
	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			waittime = timerover;
//			cmd_putmore (uart3_rxbuf,len);
//			uart3_rx_endma ();
		} 
	}
}

/*******************************************************************
函数原形： u8 esp8266_com_setspeed (u32 baud)
功能：进行smartconfig操作
参数：baud波特率110~115200*40
返回值:0完成配置，
*******************************************************************/
u8 esp8266_com_setspeed (u32 baud)
{	
	u8 res = 1;
	u32 waittime = 20,len = 0;
	
	
	u3_printf("AT+UART_CUR=%u,8,1,0,0\r\n",baud);	//发送命令
	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		len = uart3_get_data ();
		if(len)//接收到期待的应答结果
		{
			if(esp8266_check_cmd("OK"))
			{
				res = 0;
				break;
			}
			cmd_putmore (uart3_rxbuf,len);
		} 
	}
	
	return res;
}

/*******************************************************************
函数原形： u8 esp8266_init_check (void)
功能：上电esp8266检查
参数：无
返回值:0成功，其他值失败
*******************************************************************/
int esp8266_init_check (void)
{
	u32 i;
	u8 *p,res = 0;
	
	gwifi_mod = mymalloc(SRAMIN,sizeof(WIFI_MOD));
	if (gwifi_mod == NULL)
	{
		printf ("gwifi_mod memory error!!\r\n");
		while (1);
	}
	
	memset (gwifi_mod,0,sizeof(WIFI_MOD));
	
	esp8266_clear_rxbuf (30);
	
	while(esp8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
	{
		esp8266_quit_trans();//退出透传
		esp8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		printf ("Not found esp8266\r\n");
		delay_ms(800);
		if ((++i) >= 10)return -1;
	} 
	while(esp8266_send_cmd("ATE0","OK",20));//关闭回显
	
	res = esp8266_com_setspeed (ESP8266_COM_HSPEED);   //设置WIFI模块高速串口
	if (res == 0)
	{
		printf ("esp8266 uart hspeed %u bps set ok\r\n",ESP8266_COM_HSPEED);
		esp8266_io_hspeed ();                            //设置MCU高速串口
	}
	else
	{
		printf ("esp8266 uart hspeed set fail\r\n");
	}
	
	esp8266_send_cmd("AT+CWMODE?","+CWMODE:",20);	//获取网络模式
	p = esp8266_check_cmd(":");
	printf ("WIFI mode %s\r\n",ESP8266_CWMODE_TBL[*(p+1)-'1']);
	if (*(p+1) != '1')
	{
		esp8266_send_cmd("AT+CWMODE=1","OK",50);
	}
	
	esp8266_get_mac();                  //获取mac地址
	
	printf ("mac = %s\r\n",(char*)gwifi_mod->mac);
	
	res = esp8266_consta_check();
	printf ("status: %s\r\n",ESP8266_CIPSTATUS[res-'0']);
	if ((res == '2') || (res == '3') || (res == '4'))
	{
		gwifi_mod->wifi_ok = 1;
		esp8266_get_curjap ();            //查询当前连接AP信息
		esp8266_get_wanip();              //获取当前ip地址
		printf ("jap ssid = %s\r\n",(char*)gwifi_mod->jap_ssid);
		printf ("jap mac = %s\r\n",(char*)gwifi_mod->jap_mac);
		printf ("jap ch = %s\r\n",(char*)gwifi_mod->jap_ch);
		printf ("jap rssi = %s\r\n",(char*)gwifi_mod->jap_rssi);
		printf ("curr ip = %s\r\n",(char*)gwifi_mod->ip);
	}
	else
	{
		gwifi_mod->wifi_ok = 0;
	}
	
	return 0;
}

