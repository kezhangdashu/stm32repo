#ifndef	__CMD_H_
#define	__CMD_H_
#include <stdio.h>
#include "main.h"
#include "datafifo.h"

#define __DEBUG            //允许调试

#ifdef __DEBUG
#define DEBUG(format,...)  printf(format, ##__VA_ARGS__)
#else
#define DEBUG(format,...) 
#endif

#define CMD_RX_DEEP    32                         //串口接收缓冲区大小
#define CMDNUM          3		                    //命令总数

#define CONFIG_SYS_MAXARGS      3			  		//命令参数总数
#define CONFIG_SYS_CBSIZE		CMD_RX_DEEP			//命令最大长度

struct cmd_tbl_s {
	char	*name;		        //Command Name
	int	maxargs;	            //maximum number of arguments
	int (*cmd)(int, char *[]);
}; 

typedef struct cmd_tbl_s	cmd_tbl_t;

extern void cmd_putmore (u8 *psend, u32 len);
extern void cmd_putascii (u8 *psend, u32 len);

extern void cmd_init (void);
extern void cmd_event_get (void);


#endif
