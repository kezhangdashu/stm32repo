#ifndef __DRV10983_H__
#define __DRV10983_H__

#include "main.h"

#define NUMCONFIGREGS		12

extern u8 ge_ConfigRegData[NUMCONFIGREGS];

/*************** Reg 0x00 & Values ****************/
#define SPEED_CTRL1     0x00

/*************** Reg 0x01 & Values ****************/
#define SPEED_CTRL2     0x01

#define OVER_RIDE_1     0x80

/*************** Reg 0x05 & Values ****************/
#define SUPPLY_VOLTAGE				0x1A

#define SUPPLY_V_12					0x67
#define SUPPLY_V_22_83				0xBB
#define SUPPLY_V_22_75				0xF0

/*************** Reg 0x03 & Values ****************/
#define EE_CTRL						0x03

#define EE_WRITE_0					0x00
#define EE_WRITE_1					0x10

#define EE_REFRESH_0				0x00
#define EE_REFRESH_1				0x20

#define SI_DATA_0					0x00
#define SI_DATA_1					0x40


/*************** Reg 0x02 & Values ****************/
#define DEV_CTRL					0x02

#define EN_PROG_KEY					0xB6


/*************** Reg 0x88 & Values ****************/
#define DRV_83_OR_75				0x88

#define MASK_83_OR_75				0x20

/****************status reg************************/
#define MOTOR_SPEED1        0x11
#define MOTOR_SPEED2        0x12

/****************默认config eeprom*********************/
//#define REG_20_DATA		0x39
//#define REG_21_DATA		0x1E
//#define REG_22_DATA		0x3A
//#define REG_23_DATA		0x08
//#define REG_24_DATA		0x50
//#define REG_25_DATA		0xDA
//#define REG_26_DATA		0x88
//#define REG_27_DATA		0x10
//#define REG_28_DATA		0x27
//#define REG_29_DATA		0x37
//#define REG_2A_DATA		0x04
//#define REG_2B_DATA		0x0C

/****************贺总config eeprom*********************/
//#define REG_20_DATA		0x3e
//#define REG_21_DATA		0x28
//#define REG_22_DATA		0x3A
//#define REG_23_DATA		0x08
//#define REG_24_DATA		0x50
//#define REG_25_DATA		0xDA
//#define REG_26_DATA		0x88
//#define REG_27_DATA		0x10
//#define REG_28_DATA		0x98
//#define REG_29_DATA		0x37
//#define REG_2A_DATA		0x04
//#define REG_2B_DATA		0x0C

/****************config eeprom*********************/
//加入后面闭环控制以及限制，没有电流限制
#define REG_20_DATA     0x3f        //该值0x3e或者0x3f
#define REG_21_DATA     0x28
#define REG_22_DATA     0x3a
#define REG_23_DATA     0x08
#define REG_24_DATA     0x50
#define REG_25_DATA     0xda
//#define REG_26_DATA     0x88
#define REG_26_DATA     0x8b        //Align time时间缩短可以加快电机启动速度，但太快也会导致电机启动失败
#define REG_27_DATA     0xe0        //电流不能限制否则会出现速度提不起
#define REG_28_DATA     0xbe        //加速电流加入限制，限制在1A
//#define REG_29_DATA     0xb7
#define REG_29_DATA     0xb9        //24V系统死区时间最小设置400ns
#define REG_2A_DATA     0x0c
//#define REG_2B_DATA     0x0c      //SPEED模拟输入控制风速
#define REG_2B_DATA     0x0e        //SPEEDPWM输入控制风速

extern int DRV10983_init (void);
extern void DRV10983_wrspeed (u16 speed);
extern u32 DRV10983_rdspeed (void);
extern int DRV10983_75eePromRegs_write(void);
extern void DRV10983_75eePromRegs_read(void);

#endif
