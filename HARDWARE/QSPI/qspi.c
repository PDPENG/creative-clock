#include "qspi.h"
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
 *	QSPI驱动代码
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

QSPI_HandleTypeDef QSPI_Handler;    //QSPI句柄

/**
 * @brief	QSPI初始化代码，配置成主机模式
 *
 * @param   void
 *
 * @return  u8	0:初始化成功；1:初始化失败
 */
u8 QSPI_Init(void)
{
    QSPI_Handler.Instance = QUADSPI;                        //QSPI
    QSPI_Handler.Init.ClockPrescaler = 0;                   //QPSI分频比，W25Q128最大频率为104M，
    //所以此处应该为0，QSPI频率就为80/(0+1)=40MHZ
    QSPI_Handler.Init.FifoThreshold = 4;                    //FIFO阈值为4个字节
    QSPI_Handler.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE; //采样移位半个周期(DDR模式下,必须设置为0)
    QSPI_Handler.Init.FlashSize = POSITION_VAL(0x1000000) - 1; //SPI FLASH大小，W25Q128大小为16M字节
    QSPI_Handler.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE; //片选高电平时间为4个时钟(12.5*4=50ns),即手册里面的tSHSL参数
    QSPI_Handler.Init.ClockMode = QSPI_CLOCK_MODE_0;        //模式0
    if(HAL_QSPI_Init(&QSPI_Handler) == HAL_OK) return 0;    //QSPI初始化成功
    else return 1;
}
/**
 * @brief	QSPI底层驱动,引脚配置，时钟使能
 *
 * @param   hqspi	QSPI句柄
 *
 * @return  void
 */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_QSPI_CLK_ENABLE();        //使能QSPI时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();       //使能GPIOE时钟

    /**QUADSPI GPIO Configuration
    PE10     ------> QUADSPI_CLK
    PE11     ------> QUADSPI_NCS
    PE12     ------> QUADSPI_BK1_IO0
    PE13     ------> QUADSPI_BK1_IO1
    PE14     ------> QUADSPI_BK1_IO2
    PE15     ------> QUADSPI_BK1_IO3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13
                          | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
/**
 * @brief	QSPI发送命令
 *
 * @param   instruction		要发送的指令
 * @param   address			发送到的目的地址
 * @param   dummyCycles		空指令周期数
 * @param   instructionMode	指令模式;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE
 * @param   addressMode		地址模式; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
 * @param   addressSize		地址长度;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
 * @param   dataMode		数据模式; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE
 *
 * @return  void
 */
void QSPI_Send_CMD(u32 instruction, u32 address, u32 dummyCycles, u32 instructionMode, u32 addressMode, u32 addressSize, u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;

    Cmdhandler.Instruction = instruction;                 	//指令
    Cmdhandler.Address = address;                            	//地址
    Cmdhandler.DummyCycles = dummyCycles;                   //设置空指令周期数
    Cmdhandler.InstructionMode = instructionMode;				//指令模式
    Cmdhandler.AddressMode = addressMode;   					//地址模式
    Cmdhandler.AddressSize = addressSize;   					//地址长度
    Cmdhandler.DataMode = dataMode;             				//数据模式
    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;       	//每次都发送指令
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; //无交替字节
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;           	//关闭DDR模式
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    HAL_QSPI_Command(&QSPI_Handler, &Cmdhandler, 5000);
}
/**
 * @brief	QSPI接收指定长度的数据
 *
 * @param   buf				接收数据缓冲区首地址
 * @param   datalen			要传输的数据长度
 *
 * @return  u8				0:正常;其他:错误代码
 */
u8 QSPI_Receive(u8* buf, u32 datalen)
{
    QSPI_Handler.Instance->DLR = datalen - 1;                       //配置数据长度
    if(HAL_QSPI_Receive(&QSPI_Handler, buf, 5000) == HAL_OK) return 0; //接收数据
    else return 1;
}
/**
 * @brief	QSPI发送指定长度的数据
 *
 * @param   buf				发送数据缓冲区首地址
 * @param   datalen			要传输的数据长度
 *
 * @return  u8				0:正常;其他:错误代码
 */
u8 QSPI_Transmit(u8* buf, u32 datalen)
{
    QSPI_Handler.Instance->DLR = datalen - 1;                        //配置数据长度
    if(HAL_QSPI_Transmit(&QSPI_Handler, buf, 5000) == HAL_OK) return 0; //发送数据
    else return 1;
}
