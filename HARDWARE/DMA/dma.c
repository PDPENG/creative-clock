#include "dma.h"
#include "lcd.h"
#include "usart.h"

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
 *	DMA��������
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

DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA���


/**
 * @brief	DMAx�ĸ�ͨ������,����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
 *			�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
 *
 * @param   DMA_Streamx		DMA������,DMA1_Stream0~7/DMA2_Stream0~7
 * @param   chx				DMAͨ��ѡ��,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
 *
 * @return  void
 */
void MYDMA_Config(DMA_Channel_TypeDef *DMA_Streamx, u32 chx)
{
    if((u32)DMA_Streamx > (u32)DMA2) //�õ���ǰstream������DMA2����DMA1
    {
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ��
    }

    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ��
    }

    __HAL_LINKDMA(&UART1_Handler, hdmatx, UART1TxDMA_Handler);  //��DMA��USART1��ϵ����(����DMA)

    //Tx DMA����
    UART1TxDMA_Handler.Instance = DMA_Streamx;                          //������ѡ��
    UART1TxDMA_Handler.Init.Request = chx;                              //ͨ��ѡ��
    UART1TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;           //�洢��������
    UART1TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;               //���������ģʽ
    UART1TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;                   //�洢������ģʽ
    UART1TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;  //�������ݳ���:8λ
    UART1TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;     //�洢�����ݳ���:8λ
    UART1TxDMA_Handler.Init.Mode = DMA_NORMAL;                          //������ͨģʽ
    UART1TxDMA_Handler.Init.Priority = DMA_PRIORITY_HIGH;             //�е����ȼ�

    HAL_DMA_DeInit(&UART1TxDMA_Handler);
    HAL_DMA_Init(&UART1TxDMA_Handler);
}


