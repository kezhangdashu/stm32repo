#ifndef	__ESP8266_API_H_
#define	__ESP8266_API_H_
#include "main.h"

typedef __packed struct _WIFI_MOD
{
	u8 wifi_ok;                    //wifi连接成功，为1表示成功,0失败 
	u8 jap_ssid[256];              //当前加入的AP的SSID
	u8 jap_mac[32];                //当前加入的AP的MAC
	u8 jap_ch[16];                 //当前加入的AP的信道
	u8 jap_rssi[16];               //当前加入的AP的信号强度
	
	u8 ip[16];                     //当前ip地址
	u8 mac[32];                    //mac地址
}WIFI_MOD;

extern const u8 *ESP8266_CIPSTATUS[6];

extern WIFI_MOD *gwifi_mod;

extern u8 esp8266_get_wanip(void);
extern u8 esp8266_get_mac(void);
extern u8 esp8266_get_curjap (void);
extern u8 esp8266_get_gmr (u8 *pgmr);
extern u8 esp8266_join_ap (u8* ssidbuf, u8* passbuf);
extern u8 esp8266_tcp_connect (u8 *ipbuf, u8 *portbuf);
extern u8 esp8266_tcp_close (void);
extern u8 esp8266_set_send (void);
extern u8 esp8266_tcp_send (u8 *sendbuf,u32 slen);
extern u32 esp8266_tcp_recv_nowait (u8 *rxbuf);
extern u8 esp8266_smart_config (u32 timeover,u8* ssidbuf,u8* passbuf);

extern int esp8266_init_check (void);

extern u8 esp8266_consta_check(void);

extern u8 esp8266_quit_trans(void);

#endif
