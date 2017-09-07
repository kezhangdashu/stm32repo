#include <stdio.h>
#include "string.h"
#include "exfuns.h"
#include "malloc.h"


///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[_VOLUMES];//逻辑磁盘工作区.	 
//FIL *file;	  		//文件1
//FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
//FILINFO fileinfo;	//文件信息
//DIR dir;  			//目录

//u8 *fatbuf;			//SD卡数据缓存区
///////////////////////////////////////////////////////////////////////////////////////
//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//为磁盘i工作区申请内存	
		if(!fs[i])break;
	}
//	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为file申请内存
//	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为ftemp申请内存
//	fatbuf=(u8*)mymalloc(SRAMIN,512);				//为fatbuf申请内存
	if(i==_VOLUMES)return 0;  //申请有一个失败,即失败.
	else return 1;	
}      

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
//#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
//		tot_sect*=fs1->ssize/512;
//		fre_sect*=fs1->ssize/512;
//#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}	


//文件系统初始化
//返回值:0,正常.其他,错误代码
int fatfs_init (void)
{
	int res = 0;
	
	res = exfuns_init();					    //为fatfs相关变量申请内存
	if (res)
	{
		printf ("fatfs memory error!!\r\n");
		return res;
	}
	
	res = f_mount(fs[0],"0:",1); 				//挂载FLASH.	
	if(res==FR_NO_FILESYSTEM)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		printf("Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("0:",1,4096);//格式化FLASH,0,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
		//	f_setlabel((const TCHAR *)"0:");	//设置Flash磁盘的名字为：ALIENTEK
			printf ("Flash Disk Format Finish");	//格式化完成
		}
		else 
		{
			printf ("Flash Disk Format Error = %u",res);	//格式化失败
		}
	}
	else
	{
		printf ("fatfs ok\r\n");
	}
	
	return res;
}

