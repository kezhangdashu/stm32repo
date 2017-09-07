#ifndef	__ESP8266_IO_H_
#define	__ESP8266_IO_H_
#include "main.h"

#define  ESP8266_COM_HSPEED 10*115200   //WIFI串口高速波特率

#define  UART3_RX_DEEP  2048+128
#define  UART3_TX_DEEP  1024+128
#define  DMA_RX_OVTIM   10            //串口DMA接收超时时间，单位ms

extern u8 *uart3_rxbuf;
extern u8 *uart3_txbuf;
extern u8 esp8266_cmd_mode;

extern void esp8266_dma_init(void);
extern void esp8266_io_init(void);

extern void esp8266_io_hspeed (void);

extern u32 uart3_get_data (void);

extern void u3_putchar (u8 ch);
extern void u3_printf(char* fmt,...);


extern void esp8266_test (void);

#endif

