#include "stmflash.h"
#include "delay.h"

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
 *	STM32内部FLASH读写驱动代码
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


/**
 * @brief	读取指定地址的字(32位数据)
 *
 * @param   faddr	读地址
 *
 * @return  u32		读取的数据
 */
u32 STMFLASH_ReadWord(u32 faddr)
{
    return *(vu32*)faddr;
}


/**
 * @brief	识别FLASH地址所在BANK(该芯片有两个BANK)
 *
 * @param   Addr	FLASH地址
 *
 * @return  u32		FLASH地址所属BANK区
 */
static uint32_t Get_FLASH_Bank(uint32_t Addr)
{
    uint32_t bank = 0;

    /* No Bank swap */
    if(Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        bank = FLASH_BANK_1;
    }

    else
    {
        bank = FLASH_BANK_2;
    }

    return bank;
}


/**
 * @brief	不检查的写入数据(64位)
 *
 * @param  	WriteAddr	起始地址(此地址必须为8的倍数!!)
 * @param  	pBuffer		数据指针
 * @param  	NumToWrite	数据(64位)量(就是要写入的64位数据的个数)
 *
 * @return  void
 */
void STMFLASH_Write_NoCheck(u32 WriteAddr, uint64_t *pBuffer, u32 NumToWrite)
{
    u32 i;

    for(i = 0; i < NumToWrite; i++)
    {
        if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WriteAddr, pBuffer[i]) != HAL_OK )
		{
			return;		//写入错误！！！
		}
        WriteAddr += 8; //地址增加
    }
}


/**
 * @brief	从指定地址开始写入指定长度的数据，每次写入两个字数据(64位)
 *
 * @param  	WriteAddr	起始地址(此地址必须为8的倍数!!)
 * @param  	pBuffer		数据指针
 * @param  	NumToWrite	数据(64位)量(就是要写入的64位数据的个数)
 *
 * @return  void
 */
#define STM_SECTOR_SIZE	2048
uint64_t STMFLASH_BUF[STM_SECTOR_SIZE / 8]; //最多是2K字节
void STMFLASH_Write(u32 WriteAddr, uint64_t *pBuffer, u32 NumToWrite)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    uint32_t	SectorError;

    if(WriteAddr < STM32_FLASH_BASE || WriteAddr % 8)return;	//非法地址

    u16 i;
    u32 secpos;	   //扇区地址
    u16 secoff;	   //扇区内偏移地址(16位字计算)
    u16 secremain; //扇区内剩余地址(16位字计算)
    u32 offaddr;   //去掉0X08000000后的地址

    offaddr = WriteAddr - STM32_FLASH_BASE;		//实际偏移地址.
    secpos = offaddr / STM_SECTOR_SIZE;			//扇区地址
    secoff = (offaddr % STM_SECTOR_SIZE) / 8;		//在页内的偏移(8个字节为基本单位)
    secremain = STM_SECTOR_SIZE / 8 - secoff;		//扇区剩余空间大小

    if(NumToWrite <= secremain)secremain = NumToWrite; //不大于该扇区范围

    HAL_FLASH_Unlock();             //解锁

    while(1)
    {
        STMFLASH_Read(secpos * STM_SECTOR_SIZE, (u32*)STMFLASH_BUF, STM_SECTOR_SIZE / 4);	//读出该页全部数据

        for(i = 0; i < secremain; i++)
        {
            if(STMFLASH_BUF[secoff + i] != 0xFFFFFFFFFFFFFFFF) break;//有非0xFFFFFFFFFFFFFFFF的地方,要擦除这个扇区
        }

        if(i < secremain)//需要擦除
        {
            FlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES; //擦除类型，扇区擦除
            FlashEraseInit.Banks = Get_FLASH_Bank(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE); //要擦除的Bank
            FlashEraseInit.Page = secpos;    			//要擦出的页
            FlashEraseInit.NbPages = 1;      			//需要删除的页数

            if(HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
            {
                break;//发生错误了
            }

            for(i = 0; i < secremain; i++) //复制
            {
                STMFLASH_BUF[i + secoff] = pBuffer[i];
            }

            STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 8);
        }

        else//写已经擦除了的,直接写入扇区剩余区间.
        {
            STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);
        }

        if(NumToWrite == secremain)break; //写入结束了

        else
        {
            secpos++;				//扇区地址增1
            secoff = 0;				//偏移位置为0
            pBuffer += secremain;  	//指针偏移
            WriteAddr += (secremain * 8); //写地址偏移
            NumToWrite -= secremain;	//字节(64位)数递减

            if(NumToWrite > (STM_SECTOR_SIZE / 8))secremain = STM_SECTOR_SIZE / 8; //下一个扇区还是写不完

            else secremain = NumToWrite; //下一个扇区可以写完了
        }
    }

    HAL_FLASH_Lock();           //上锁
}

/**
 * @brief	从指定地址开始读出指定长度的数据，每次读出一个字(32位)
 *
 * @param  	ReadAddr	起始地址
 * @param  	pBuffer		数据指针
 * @param  	NumToRead	读出字(32位)数
 *
 * @return  void
 */
void STMFLASH_Read(u32 ReadAddr, u32 *pBuffer, u32 NumToRead)
{
    u32 i;

    for(i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); //读取4个字节.
        ReadAddr += 4; //偏移4个字节.
    }
}




