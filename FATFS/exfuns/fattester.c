#include "fattester.h"	 
#include "sd_card.h"
#include "usmart.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"
#include "string.h"


/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	FATFS ���Դ���
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/


/**
 * @brief	Ϊ����ע�Ṥ����
 *
 * @param   path	����·��������"0:"��"1:"
 * @param   mt		0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
 *
 * @return  u8		ִ�н��
 */
u8 mf_mount(u8* path,u8 mt)
{		   
	return f_mount(fs[1],(const TCHAR*)path,mt); 
}

/**
 * @brief	��·���µ��ļ�
 *
 * @param   path	·��+�ļ���
 * @param   mode	��ģʽ
 *
 * @return  u8		ִ�н��
 */
u8 mf_open(u8*path,u8 mode)
{
	u8 res;	 
	res=f_open(file,(const TCHAR*)path,mode);//���ļ���
	return res;
}

/**
 * @brief	�ر��ļ�
 *
 * @param   void
 *
 * @return  u8		ִ�н��
 */
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

/**
 * @brief	��������
 *
 * @param   len		�����ĳ���
 *
 * @return  u8		ִ�н��
 */
u8 mf_read(u16 len)
{
	u16 i,t;
	u8 res=0;
	u16 tlen=0;
	printf("\r\nRead file data is:\r\n");
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			printf("Read Error:%d\r\n",res);
			break;
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
		if(res)	//�����ݳ�����
		{
			printf("\r\nRead Error:%d\r\n",res);   
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}	 
	}
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
	printf("Read data over\r\n");	 
	return res;
}

/**
 * @brief	д������
 *
 * @param   dat		���ݻ�����
 * @param   len		д�볤��
 *
 * @return  u8		ִ�н��
 */
u8 mf_write(u8*dat,u16 len)
{			    
	u8 res;	   					   

	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);	 
	res=f_write(file,dat,len,&bw);
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");
	return res;
}


/**
 * @brief	��Ŀ¼
 *
 * @param   path	·��
 *
 * @return  u8		ִ�н��
 */
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}


/**
 * @brief	�ر�Ŀ¼
 *
 * @param   void
 *
 * @return  u8		ִ�н��
 */
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}

/**
 * @brief	���ȡ�ļ���
 *
 * @param   void
 *
 * @return  u8		ִ�н��
 */
u8 mf_readdir(void)
{
	u8 res;    
	res=f_readdir(&dir,&fileinfo);	//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK)return res;		//������ 
	printf("\r\n DIR info:\r\n");

	printf("dir.dptr:%d\r\n",dir.dptr);
	printf("dir.obj.id:%d\r\n",dir.obj.id);
	printf("dir.obj.sclust:%d\r\n",dir.obj.sclust);
	printf("dir.obj.objsize:%lld\r\n",dir.obj.objsize);
	printf("dir.obj.c_ofs:%d\r\n",dir.obj.c_ofs);
	printf("dir.clust:%d\r\n",dir.clust);
	printf("dir.sect:%d\r\n",dir.sect);	  
	printf("dir.blk_ofs:%d\r\n",dir.blk_ofs);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",fileinfo.fname);
	printf("File Size is:%lld\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n"); 
	return 0;
}			 

/**
 * @brief	�����ļ�
 *
 * @param   path	·��
 *
 * @return  u8		ִ�н��
 */
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	 
    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
 			printf("%s/", path);//��ӡ·��	
			printf("%s\r\n",fileinfo.fname);//��ӡ�ļ���	  
		} 
    }	   
    return res;	  
}


/**
 * @brief	��ʾʣ������
 *
 * @param   drv		�̷�
 *
 * @return  u8		ʣ������(�ֽ�)
 */
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//������С��10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}

/**
 * @brief	�ļ���дָ��ƫ��
 *
 * @param   offset	����׵�ַ��ƫ����
 *
 * @return  u8		ִ�н��
 */
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}

/**
 * @brief	��ȡ�ļ���ǰ��дָ���λ��
 *
 * @param   void
 *
 * @return  u32		λ��
 */
u32 mf_tell(void)
{
	return f_tell(file);
}
/**
 * @brief	��ȡ�ļ���С
 *
 * @param   void
 *
 * @return  u32		�ļ���С
 */
u32 mf_size(void)
{
	return f_size(file);
}

/**
 * @brief	����Ŀ¼
 *
 * @param   pname	Ŀ¼·��+����
 *
 * @return  u8		ִ�н��
 */
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
/**
 * @brief	��ʽ��
 *
 * @param   path	����·��������"0:"��"1:"
 * @param   mode	ģʽ
 * @param   au		�ش�С
 *
 * @return  u8		ִ�н��
 */
u8 mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
}
/**
 * @brief	ɾ���ļ�/Ŀ¼
 *
 * @param   pname	�ļ�/Ŀ¼·��+����
 *
 * @return  u8		ִ�н��
 */
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}
/**
 * @brief	�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
 *
 * @param   oldname:֮ǰ������
 * @param   newname:������
 *
 * @return  u8		ִ�н��
 */
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
/**
 * @brief	��ȡ�̷����������֣�
 *
 * @param   path	����·��������"0:"��"1:"  
 *
 * @return  void
 */
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn=0;
	u8 res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		printf("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
		printf("����%s �����к�:%X\r\n\r\n",path,sn); 
	}else printf("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
}
/**
 * @brief	�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
 *
 * @param   path	���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"   
 *
 * @return  void
 */
void mf_setlabel(u8 *path)
{
	u8 res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		printf("\r\n�����̷����óɹ�:%s\r\n",path);
	}else printf("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
} 
/**
 * @brief	���ļ������ȡһ���ַ���
 *
 * @param   size	Ҫ��ȡ�ĳ���
 *
 * @return  void
 */
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//û�����ݶ���
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}

/**
 * @brief	��Ҫ_USE_STRFUNC>=1,дһ���ַ����ļ�
 *
 * @param   c:		Ҫд����ַ�
 *
 * @return  u8		ִ�н��
 */
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
/**
 * @brief	д�ַ������ļ�
 *
 * @param   c:		Ҫд����ַ���
 *
 * @return  u8		д����ַ�������
 */
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}













