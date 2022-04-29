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
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	DMA驱动代码
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

DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA句柄


/**
 * @brief	DMAx的各通道配置,这里的传输形式是固定的,这点要根据不同的情况来修改
 *			从存储器->外设模式/8位数据宽度/存储器增量模式
 *
 * @param   DMA_Streamx		DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
 * @param   chx				DMA通道选择,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
 *
 * @return  void
 */
void MYDMA_Config(DMA_Channel_TypeDef *DMA_Streamx, u32 chx)
{
    if((u32)DMA_Streamx > (u32)DMA2) //得到当前stream是属于DMA2还是DMA1
    {
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2时钟使能
    }

    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能
    }

    __HAL_LINKDMA(&UART1_Handler, hdmatx, UART1TxDMA_Handler);  //将DMA与USART1联系起来(发送DMA)

    //Tx DMA配置
    UART1TxDMA_Handler.Instance = DMA_Streamx;                          //数据流选择
    UART1TxDMA_Handler.Init.Request = chx;                              //通道选择
    UART1TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;           //存储器到外设
    UART1TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;               //外设非增量模式
    UART1TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;                   //存储器增量模式
    UART1TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    UART1TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;     //存储器数据长度:8位
    UART1TxDMA_Handler.Init.Mode = DMA_NORMAL;                          //外设普通模式
    UART1TxDMA_Handler.Init.Priority = DMA_PRIORITY_HIGH;             //中等优先级

    HAL_DMA_DeInit(&UART1TxDMA_Handler);
    HAL_DMA_Init(&UART1TxDMA_Handler);
}


