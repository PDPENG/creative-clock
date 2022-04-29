/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "diskio.h"			/* FatFs lower layer API */
#include "sd_card.h"
#include "w25qxx.h"
#include "malloc.h"	 

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
 *	FATFS�ײ�(diskio)��������
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
 

#define SD_CARD	 	0  			//SD��,���Ϊ0
#define EX_FLASH 	1			//�ⲿspi flash,���Ϊ1

//����W25Q128
//���7M�����fatfs��
#define FLASH_SECTOR_SIZE 	512	
#define FLASH_SECTOR_COUNT 	1024*7*2	//W25Q128,���7M�ֽڸ�FATFSռ��	
#define FLASH_BLOCK_SIZE   	8     		//ÿ��BLOCK��8������		
  
#define FLASH_Offset_Addr	((16-7)*1024*1024)	//���7M�ռ��FATFSʹ��

/**
 * @brief	��ȡ����״̬
 *
 * @param   pdrv	���̱��
 *
 * @return  u8		RES_OK
 */
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}  
/**
 * @brief	��ʼ������
 *
 * @param   pdrv	���̱��
 *
 * @return  u8		0:��ʼ���ɹ���1:��ʼ��ʧ��
 */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD:		//SD��
			res=SD_Init();	//SD����ʼ�� 
  			break;
		case EX_FLASH:		//�ⲿflash
			W25QXX_Init();  //W25QXX��ʼ��
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //��ʼ���ɹ� 
}

/**
 * @brief	������
 *
 * @param   pdrv	���̱��
 * @param   buff	���ݽ��ջ����׵�ַ
 * @param   sector	������ַ
 * @param   count	��Ҫ��ȡ��������
 *
 * @return  DRESULT	RES_OK:�ɹ���RES_ERROR:ʧ��
 */
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_ReadDisk(buff,sector,count);	 
			while(res)//������
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_ReadDisk(buff,sector,count);	
				//printf("sd rd error:%d\r\n",res);
			}
			break;
		case EX_FLASH://�ⲿflash
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE+FLASH_Offset_Addr,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		default:
			res=1; 
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

/**
 * @brief	д����
 *
 * @param   pdrv	���̱��0~9
 * @param   buff	���������׵�ַ
 * @param   sector	������ַ
 * @param   count	��Ҫд��������� 
 *
 * @return  DRESULT	RES_OK:�ɹ���RES_ERROR:ʧ��
 */
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res=0;  
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_WriteDisk((u8*)buff,sector,count);
			while(res)//д����
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_WriteDisk((u8*)buff,sector,count);	
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		case EX_FLASH://�ⲿflash
			for(;count>0;count--)
			{										    
				W25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE+FLASH_Offset_Addr,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		default:
			res=1; 
	}
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}

/**
 * @brief	�����豸ָ�����Ժͳ��˶�/д��������
 *
 * @param   pdrv	���̱��0~9
 * @param   ctrl	ָ���������
 * @param   buff	ָ������������׵�ַ,ȡ��������,�޲���ʱ,ָ�� NULL 
 *
 * @return  DRESULT	��ο�DRESULT����
 */
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
DRESULT res;						  			     
	if(pdrv==SD_CARD)//SD��
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==EX_FLASH)	//�ⲿFLASH  
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res=RES_ERROR;//�����Ĳ�֧��
    return res;
}

/**
 * @brief	Ϊfatfsģ���ṩ��ǰʱ��
 *			31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)                                                                                                                                                                                                                                           
 *			15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)  
 *
 * @param   pdrv	���̱��0~9
 * @param   ctrl	ָ���������
 * @param   buff	ָ������������׵�ַ,ȡ��������,�޲���ʱ,ָ�� NULL 
 *
 * @return  DWORD	ʱ������
 */                                                                                                                                                                                                                                          
DWORD get_fattime (void)
{				 
	return 0;
}

/**
 * @brief	��̬�����ڴ�  
 *
 * @param   size	�����ڴ��С
 *
 * @return  void*	���䵽���ڴ��׵�ַ
 */
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAM1,size);
}
/**
 * @brief	�ͷ��ڴ�  
 *
 * @param   mf		�ڴ��׵�ַ
 *
 * @return  void
 */
void ff_memfree (void* mf)		 
{
	myfree(SRAM1,mf);
}









