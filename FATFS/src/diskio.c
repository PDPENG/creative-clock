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
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	FATFS底层(diskio)驱动代码
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/
 

#define SD_CARD	 	0  			//SD卡,卷标为0
#define EX_FLASH 	1			//外部spi flash,卷标为1

//对于W25Q128
//最后7M分配给fatfs用
#define FLASH_SECTOR_SIZE 	512	
#define FLASH_SECTOR_COUNT 	1024*7*2	//W25Q128,最后7M字节给FATFS占用	
#define FLASH_BLOCK_SIZE   	8     		//每个BLOCK有8个扇区		
  
#define FLASH_Offset_Addr	((16-7)*1024*1024)	//最后7M空间给FATFS使用

/**
 * @brief	获取磁盘状态
 *
 * @param   pdrv	磁盘编号
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
 * @brief	初始化磁盘
 *
 * @param   pdrv	磁盘编号
 *
 * @return  u8		0:初始化成功，1:初始化失败
 */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD:		//SD卡
			res=SD_Init();	//SD卡初始化 
  			break;
		case EX_FLASH:		//外部flash
			W25QXX_Init();  //W25QXX初始化
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功 
}

/**
 * @brief	读扇区
 *
 * @param   pdrv	磁盘编号
 * @param   buff	数据接收缓冲首地址
 * @param   sector	扇区地址
 * @param   count	需要读取的扇区数
 *
 * @return  DRESULT	RES_OK:成功，RES_ERROR:失败
 */
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡
			res=SD_ReadDisk(buff,sector,count);	 
			while(res)//读出错
			{
				SD_Init();	//重新初始化SD卡
				res=SD_ReadDisk(buff,sector,count);	
				//printf("sd rd error:%d\r\n",res);
			}
			break;
		case EX_FLASH://外部flash
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
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

/**
 * @brief	写扇区
 *
 * @param   pdrv	磁盘编号0~9
 * @param   buff	发送数据首地址
 * @param   sector	扇区地址
 * @param   count	需要写入的扇区数 
 *
 * @return  DRESULT	RES_OK:成功，RES_ERROR:失败
 */
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res=0;  
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡
			res=SD_WriteDisk((u8*)buff,sector,count);
			while(res)//写出错
			{
				SD_Init();	//重新初始化SD卡
				res=SD_WriteDisk((u8*)buff,sector,count);	
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		case EX_FLASH://外部flash
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
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}

/**
 * @brief	控制设备指定特性和除了读/写外的杂项功能
 *
 * @param   pdrv	磁盘编号0~9
 * @param   ctrl	指定命令代码
 * @param   buff	指向参数缓冲区首地址,取决于命令,无参数时,指向 NULL 
 *
 * @return  DRESULT	请参考DRESULT定义
 */
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
DRESULT res;						  			     
	if(pdrv==SD_CARD)//SD卡
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
	}else if(pdrv==EX_FLASH)	//外部FLASH  
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
	}else res=RES_ERROR;//其他的不支持
    return res;
}

/**
 * @brief	为fatfs模块提供当前时间
 *			31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)                                                                                                                                                                                                                                           
 *			15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)  
 *
 * @param   pdrv	磁盘编号0~9
 * @param   ctrl	指定命令代码
 * @param   buff	指向参数缓冲区首地址,取决于命令,无参数时,指向 NULL 
 *
 * @return  DWORD	时间数据
 */                                                                                                                                                                                                                                          
DWORD get_fattime (void)
{				 
	return 0;
}

/**
 * @brief	动态分配内存  
 *
 * @param   size	申请内存大小
 *
 * @return  void*	分配到的内存首地址
 */
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAM1,size);
}
/**
 * @brief	释放内存  
 *
 * @param   mf		内存首地址
 *
 * @return  void
 */
void ff_memfree (void* mf)		 
{
	myfree(SRAM1,mf);
}









