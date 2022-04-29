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


//�ڴ��(32�ֽڶ���)
__align(32) u8 mem1base[MEM1_MAX_SIZE];													//�ڲ�SRAM1�ڴ��
__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X10000000)));					//�ڲ�SRAM2�ڴ��
//�ڴ�����
u32 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													//�ڲ�SRAM1�ڴ��MAP
u32 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X10000000 + MEM2_MAX_SIZE)));	//�ڲ�SRAM2�ڴ��MAP
//�ڴ�������
const u32 memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE};			//�ڴ���С
const u32 memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE};						//�ڴ�ֿ��С
const u32 memsize[SRAMBANK] = {MEM1_MAX_SIZE, MEM2_MAX_SIZE};								//�ڴ��ܴ�С

//�ڴ���������
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,						//�ڴ��ʼ��
    my_mem_perused,						//�ڴ�ʹ����
    mem1base, mem2base,			//�ڴ��
    mem1mapbase, mem2mapbase, //�ڴ����״̬��
    0, 0,  		 					//�ڴ����δ����
};

/**
 * @brief	�����ڴ�
 *
 * @param   *des	Ŀ�ĵ�ַ
 * @param   *stc	Դ��ַ
 * @param   n		��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
 *
 * @return  void
 */
void mymemcpy(void *des, void *src, u32 n)
{
    u8 *xdes = des;
    u8 *xsrc = src;

    while(n--)*xdes++ = *xsrc++;
}

/**
 * @brief	�����ڴ�
 *
 * @param   *s		�ڴ��׵�ַ
 * @param   c		Ҫ���õ�ֵ
 * @param   count	��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
 *
 * @return  void
 */
void mymemset(void *s, u8 c, u32 count)
{
    u8 *xs = s;

    while(count--)*xs++ = c;
}

/**
 * @brief	�ڴ�����ʼ��
 *
 * @param   memx	�����ڴ��
 *
 * @return  void
 */
void my_mem_init(u8 memx)
{
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 4);	//�ڴ�״̬����������
    mallco_dev.memrdy[memx] = 1;								//�ڴ�����ʼ��OK
}

/**
 * @brief	��ȡ�ڴ�ʹ����
 *
 * @param   memx	�����ڴ��
 *
 * @return  u16		ʹ����(������10��,0~1000,����0.0%~100.0%)
 */
u16 my_mem_perused(u8 memx)
{
    u32 used = 0;
    u32 i;

    for(i = 0; i < memtblsize[memx]; i++)
    {
        if(mallco_dev.memmap[memx][i])used++;
    }

    return (used * 1000) / (memtblsize[memx]);
}

/**
 * @brief	�ڴ����(�ڲ�����)
 *
 * @param   memx	�����ڴ��
 * @param   size	Ҫ������ڴ��С(�ֽ�)
 *
 * @return  u32		0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ
 */
u32 my_mem_malloc(u8 memx, u32 size)
{
    signed long offset = 0;
    u32 nmemb;	//��Ҫ���ڴ����
    u32 cmemb = 0; //�������ڴ����
    u32 i;

    if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);//δ��ʼ��,��ִ�г�ʼ��

    if(size == 0)return 0XFFFFFFFF; //����Ҫ����

    nmemb = size / memblksize[memx];  	//��ȡ��Ҫ����������ڴ����

    if(size % memblksize[memx])nmemb++;

    for(offset = memtblsize[memx] - 1; offset >= 0; offset--) //���������ڴ������
    {
        if(!mallco_dev.memmap[memx][offset])cmemb++;//�������ڴ��������

        else cmemb = 0;								//�����ڴ������

        if(cmemb == nmemb)							//�ҵ�������nmemb�����ڴ��
        {
            for(i = 0; i < nmemb; i++)  					//��ע�ڴ��ǿ�
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }

            return (offset * memblksize[memx]); //����ƫ�Ƶ�ַ
        }
    }

    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��
}

/**
 * @brief	�ͷ��ڴ�(�ڲ�����)
 *
 * @param   memx	�����ڴ��
 * @param   offset	�ڴ��ַƫ��
 *
 * @return  u8		0,�ͷųɹ�;1,�ͷ�ʧ��;
 */
u8 my_mem_free(u8 memx, u32 offset)
{
    int i;

    if(!mallco_dev.memrdy[memx])//δ��ʼ��,��ִ�г�ʼ��
    {
        mallco_dev.init(memx);
        return 1;//δ��ʼ��
    }

    if(offset < memsize[memx]) //ƫ�����ڴ����.
    {
        int index = offset / memblksize[memx];			//ƫ�������ڴ�����
        int nmemb = mallco_dev.memmap[memx][index];	//�ڴ������

        for(i = 0; i < nmemb; i++)  						//�ڴ������
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }

        return 0;
    }

    else return 2; //ƫ�Ƴ�����.
}

/**
 * @brief	�ͷ��ڴ�(�ⲿ����)
 *
 * @param   memx	�����ڴ��
 * @param   ptr		�ڴ��׵�ַ
 *
 * @return  void
 */
void myfree(u8 memx, void *ptr)
{
    u32 offset;

    if(ptr == NULL)return; //��ַΪ0.

    offset = (u32)ptr - (u32)mallco_dev.membase[memx];
    my_mem_free(memx, offset);	//�ͷ��ڴ�
}

/**
 * @brief	�����ڴ�(�ⲿ����)
 *
 * @param   memx	�����ڴ��
 * @param   size	�ڴ��С(�ֽ�)
 *
 * @return  void*	���䵽���ڴ��׵�ַ
 */
void *mymalloc(u8 memx, u32 size)
{
    u32 offset;
    offset = my_mem_malloc(memx, size);

    if(offset == 0XFFFFFFFF)return NULL;

    else return (void*)((u32)mallco_dev.membase[memx] + offset);
}

/**
 * @brief	���·����ڴ�(�ⲿ����)
 *
 * @param   memx	�����ڴ��
 * @param   *ptr	���ڴ��׵�ַ
 * @param   size	Ҫ������ڴ��С(�ֽ�)
 *
 * @return  void*	�·��䵽���ڴ��׵�ַ
 */
void *myrealloc(u8 memx, void *ptr, u32 size)
{
    u32 offset;
    offset = my_mem_malloc(memx, size);

    if(offset == 0XFFFFFFFF)return NULL;

    else
    {
        mymemcpy((void*)((u32)mallco_dev.membase[memx] + offset), ptr, size);	//�������ڴ����ݵ����ڴ�
        myfree(memx, ptr);  											  		//�ͷž��ڴ�
        return (void*)((u32)mallco_dev.membase[memx] + offset);  				//�������ڴ��׵�ַ
    }
}












