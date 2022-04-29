#ifndef __MALLOC_H
#define __MALLOC_H
#include "sys.h"

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
 *	�ڴ�������
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


#ifndef NULL
#define NULL 0
#endif

//���������ڴ��
#define SRAM1	0		//�ڲ��ڴ��SRAM1
#define SRAM2  	1		//�ڲ��ڴ��SRAM2

#define SRAMBANK 	2	//����֧�ֵ�SRAM����.	

//mem1�ڴ�����趨.mem1�����ڲ�SRAM1����.
#define MEM1_BLOCK_SIZE			64  	  						//�ڴ���СΪ64�ֽ�
#define MEM1_MAX_SIZE			64*1024  						//SRAM2�ռ�Ϊ96K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С

//mem2�ڴ�����趨.mem2�����ڲ�SRAM2����.
#define MEM2_BLOCK_SIZE			64  	  						//�ڴ���СΪ64�ֽ�
#define MEM2_MAX_SIZE			30 *1024  						//SRAM2�ռ�Ϊ32K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//�ڴ���С


//�ڴ���������
struct _m_mallco_dev
{
    void (*init)(u8);					//��ʼ��
    u16(*perused)(u8);		  	    	//�ڴ�ʹ����
    u8 	*membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
    u32 *memmap[SRAMBANK]; 				//�ڴ����״̬��
    u8  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s, u8 c, u32 count);	//�����ڴ�
void mymemcpy(void *des, void *src, u32 n); //�����ڴ�
void my_mem_init(u8 memx);				//�ڴ�����ʼ������(��/�ڲ�����)
u32 my_mem_malloc(u8 memx, u32 size);	//�ڴ����(�ڲ�����)
u8 my_mem_free(u8 memx, u32 offset);		//�ڴ��ͷ�(�ڲ�����)
u16 my_mem_perused(u8 memx) ;			//����ڴ�ʹ����(��/�ڲ�����)
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(u8 memx, void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(u8 memx, u32 size);			//�ڴ����(�ⲿ����)
void *myrealloc(u8 memx, void *ptr, u32 size); //���·����ڴ�(�ⲿ����)
#endif













