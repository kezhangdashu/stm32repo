#include <stdio.h>
#include "string.h"
#include "exfuns.h"
#include "malloc.h"


///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs[_VOLUMES];//�߼����̹�����.	 
//FIL *file;	  		//�ļ�1
//FIL *ftemp;	  		//�ļ�2.
UINT br,bw;			//��д����
//FILINFO fileinfo;	//�ļ���Ϣ
//DIR dir;  			//Ŀ¼

//u8 *fatbuf;			//SD�����ݻ�����
///////////////////////////////////////////////////////////////////////////////////////
//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�	
		if(!fs[i])break;
	}
//	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
//	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊftemp�����ڴ�
//	fatbuf=(u8*)mymalloc(SRAMIN,512);				//Ϊfatbuf�����ڴ�
	if(i==_VOLUMES)return 0;  //������һ��ʧ��,��ʧ��.
	else return 1;	
}      

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
//#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
//		tot_sect*=fs1->ssize/512;
//		fre_sect*=fs1->ssize/512;
//#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	


//�ļ�ϵͳ��ʼ��
//����ֵ:0,����.����,�������
int fatfs_init (void)
{
	int res = 0;
	
	res = exfuns_init();					    //Ϊfatfs��ر��������ڴ�
	if (res)
	{
		printf ("fatfs memory error!!\r\n");
		return res;
	}
	
	res = f_mount(fs[0],"0:",1); 				//����FLASH.	
	if(res==FR_NO_FILESYSTEM)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		printf("Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("0:",1,4096);//��ʽ��FLASH,0,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
		//	f_setlabel((const TCHAR *)"0:");	//����Flash���̵�����Ϊ��ALIENTEK
			printf ("Flash Disk Format Finish");	//��ʽ�����
		}
		else 
		{
			printf ("Flash Disk Format Error = %u",res);	//��ʽ��ʧ��
		}
	}
	else
	{
		printf ("fatfs ok\r\n");
	}
	
	return res;
}

