#ifndef __EXFUNS_H
#define __EXFUNS_H 					   
#include "main.h"
#include "ff.h"

extern FATFS *fs[_VOLUMES];  
//extern FIL *file;	 
//extern FIL *ftemp;	 
extern UINT br,bw;
//extern FILINFO fileinfo;
//extern DIR dir;
//extern u8 *fatbuf;//SD�����ݻ�����


extern u8 exfuns_init(void);							//�����ڴ�
extern u8 exf_getfree(u8 *drv,u32 *total,u32 *free);	//�õ�������������ʣ������

extern int fatfs_init (void);

#endif


