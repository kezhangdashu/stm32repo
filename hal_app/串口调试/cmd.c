/**********************************************************/
//公司名称：
//创建时间：2017-7-6
//作者：ke
//文件名：cmd.c
//功能描述：该文件提供了指令发送接收	
/**********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "message.h"
#include "usart.h"
#include "cmd.h"
#include "datafifo.h"
#include "sysparam.h"

#define CTL_CH(c)		((c) - 'a' + 1)

DATAFIFO cmd_fifo;				          //定义调试串口fifo
u8 cmd_rxbuf[CMD_RX_DEEP];		                //定义调试串口接收缓冲区
u8 cmd_cnt = 0;

int do_help_info (int argc, char *argv[]);
int do_set_readytim (int argc, char *argv[]);
int do_set_heatstop (int argc, char *argv[]);

/*1*/
cmd_tbl_t cmd_help_info = {
	"help",
	1,
	do_help_info,
};

/*2*/
cmd_tbl_t cmd_set_readytim = {
	"setrdytim",
	2,
	do_set_readytim,
};

/*3*/
cmd_tbl_t cmd_set_heatstop = {
	"sethtstop",
	2,
	do_set_heatstop,
};

cmd_tbl_t *cmd_tbl_tp[CMDNUM] = {
	&cmd_help_info,	
	&cmd_set_readytim,
	&cmd_set_heatstop,
};

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
//_sys_exit(int x) 
//{ 
//	x = x; 
//} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
//	u8 tmp = ch;
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕  
	USART2->DR = (u8) ch;
//	HAL_UART_Transmit(&huart2,&tmp,1,0xffff);
	return ch;
}
#endif 

void cmd_putmore (u8 *psend, u32 len)
{
	u32 i;
	for (i = 0; i < len; i++)
	{
		while((USART2->SR&0X40)==0);//循环发送,直到发送完毕  
		USART2->DR = psend[i];
	}
}

void cmd_putascii (u8 *psend, u32 len)
{
	u32 i;
	for (i = 0; i < len; i++)
	{
		printf ("0x%2.2x,",psend[i]);
	}
	printf ("\r\n");
}

void USART2_IRQHandler(void)
{
 
	if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  {
		insert_data_fifo (&cmd_fifo,huart2.Instance->DR);
	}
}


/*******************************************************************
函数原形： void cmd_init (void)
功能：    初始化cmd
*******************************************************************/
void cmd_init (void)
{
#ifdef __DEBUG
	cmd_cnt = 0;
	init_data_fifo (&cmd_fifo);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
#endif
}


cmd_tbl_t *find_cmd (const char *cmd)
{
	cmd_tbl_t *cmdtp;
	int len;
	int table_len;

	for (table_len = 0; table_len < CMDNUM; table_len++)
	{
		cmdtp = cmd_tbl_tp[table_len];
		len = strlen (cmdtp->name);
		if (len == strlen (cmd))
		{
			if (strncmp (cmd, cmdtp->name, len) == 0) return cmdtp;	
		}
	}

	return NULL;	/* not found or ambiguous command */		
}

int parse_line (char *lines, char *argv[])
{
	int nargs = 0;

	while (nargs < CONFIG_SYS_MAXARGS) {

		/* skip any white space */
		while ((*lines == ' ') || (*lines == '\t')) {
			++lines;
		}

		if (*lines == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;

			return (nargs);
		}

		argv[nargs++] = lines;	/* begin of argument string	*/

		/* find end of string */
		while (*lines && (*lines != ' ') && (*lines != '\t')) {
			++lines;
		}

		if (*lines == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;

			return (nargs);
		}

		*lines++ = '\0';		/* terminate current arg	 */
	}

	printf ("** Too many args (max. %d) **\r\n", CONFIG_SYS_MAXARGS);

	return (nargs);
}


int run_command (const char *cmd)
{
	cmd_tbl_t *cmdtp;
	static char cmdbuf[CONFIG_SYS_CBSIZE];	/* working copy of cmd		*/
	static char *argv[CONFIG_SYS_MAXARGS + 1];	/* NULL terminated	*/
	static int argc;


	if (!cmd || !*cmd) {
		return -1;	/* empty command */
	}

	strcpy (cmdbuf, cmd);

	
	/* Extract arguments */
	if ((argc = parse_line (cmdbuf, argv)) == 0) {
		return -1;	/* no command at all */
	}


	/* Look up command in command table */
	if ((cmdtp = find_cmd(argv[0])) == NULL) {
		printf ("unknown command '%s' - try 'help'\r\n", argv[0]);
		return -1;	/* give up after bad command */
	}

	/* OK - call function to do the command */
	
	if ((cmdtp->cmd) (argc, argv) != 0) {
		return -1;
	}


	return 0;
}


/*******************************************************************
函数原形： int do_help_info (int argc, char *argv[])
功能：   帮助命令操作
参数：argc参数个数，argv参数存储区
返回：-1命令执行失败，0命令执行成功
*******************************************************************/
int do_help_info (int argc, char *argv[])
{

	if (argc == 1)
	{
		printf ("*********************************************************************************\r\n");
		printf ("CMD            PAR1               PAR2           MARK\r\n");
		printf ("help           NULL               NULL           display help info\r\n");
		printf ("setrdytim      <%u sec~%u sec>    NULL           set fire ready over time\r\n",RDYOV_TIM_MIN/1000,RDYOV_TIM_MAX/1000);
		printf ("sethtstop      <%u C'~%u C'>      NULL           set fire heat stop temperature\r\n",HEAT_STOP_MIN,HEAT_STOP_MAX);
		printf ("*********************************************************************************\r\n");
		return 0;
	}
	return -1;
}

/*******************************************************************
函数原形： int do_set_readytim (int argc, char *argv[])
功能：设置准备时间
参数：argc参数个数，argv参数存储区
返回：-1命令执行失败，0命令执行成功
*******************************************************************/
int do_set_readytim (int argc, char *argv[])
{
	u32 t;
	
	if (argc ==2)
	{
		t = strtol (argv[1],NULL,10);
		if ((t >= (RDYOV_TIM_MIN/1000)) && (t <= (RDYOV_TIM_MAX/1000)))
		{
			gsysp.ready_tim = t*1000;
			sysparam_save ();
			printf ("set ok\r\n");
		}
		else
		{
			printf ("data overrun!!\r\n");
		}
		return 0;
	}
	return -1;
}

/*******************************************************************
函数原形： int do_set_heatstop (int argc, char *argv[])
功能：设置停止加热时间
参数：argc参数个数，argv参数存储区
返回：-1命令执行失败，0命令执行成功
*******************************************************************/
int do_set_heatstop (int argc, char *argv[])
{
	u32 t;
	
	if (argc ==2)
	{
		t = strtol (argv[1],NULL,10);
		if ((t >= HEAT_STOP_MIN) && (t <= HEAT_STOP_MAX))
		{
			gsysp.heat_stop = t;
			sysparam_save ();
			printf ("set ok\r\n");
		}
		else
		{
			printf ("data overrun!!\r\n");
		}
		return 0;
	}
	return -1;
}


/*******************************************************************
函数原形： void cmd_event_get (void)
功能：    从命令寄存区读出完整的命令并进行判断；
*******************************************************************/
void cmd_event_get (void)
{
#ifdef __DEBUG
	u8 sd;
	u8 *p = cmd_rxbuf; 

	if (delete_data_fifo(&cmd_fifo, &sd) == 0)
	{
		switch (sd)
		{
			case 0x08:{				  /*接收到退格*/
				if (cmd_cnt)
				{
					cmd_cnt--;
					putchar (0x08);
					putchar (' ');
					putchar (0x08);
				}
			}break;

			case '\n':
			case '\r':{
				if (cmd_cnt)
				{
					printf ("\r\n");
					p[cmd_cnt] = '\0';
					run_command ((const char *)p);
				}
				cmd_cnt = 0;
			}break;

			case CTL_CH('c'):{						  /*ctrl+c*/
			}break;

		  default:{
				if (cmd_cnt < CONFIG_SYS_CBSIZE-1)
				{
					p[cmd_cnt] = sd;
					putchar (sd);
					cmd_cnt++;
				}		
			}break;
		}
	}
#endif
}
