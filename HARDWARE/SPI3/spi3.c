#include "spi3.h"
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
 *	SPI3驱动代码
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

SPI_HandleTypeDef SPI3_Handler;  //SPI3句柄


/**
 * @brief	SPI3初始化代码，配置成主机模式
 *
 * @param   void
 *
 * @return  void
 */
void SPI3_Init(void)
{
    SPI3_Handler.Instance=SPI3;                         //SPI3
    SPI3_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI3_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI3_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI3_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //串行同步时钟的空闲状态为高电平
    SPI3_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI3_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI3_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//定义波特率预分频的值:波特率预分频值为256
    SPI3_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI3_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI3_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI3_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI3_Handler);//初始化SPI3
    
    __HAL_SPI_ENABLE(&SPI3_Handler);                    //使能SPI3
}

/**
 * @brief	SPI1与SPI3底层驱动，时钟使能，引脚配置
 *
 * @param   hspi	SPI句柄
 *
 * @return  void
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	if( hspi->Instance == SPI1 )
	{
		GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOA_CLK_ENABLE();       //使能GPIOA时钟
		__HAL_RCC_SPI1_CLK_ENABLE();        //使能SPI1时钟

		//PA5.6.7
		GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz           
		GPIO_Initure.Alternate=GPIO_AF5_SPI1;           //复用为SPI1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	}
	
	if( hspi->Instance == SPI3 )
	{
		GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOB时钟
		__HAL_RCC_SPI3_CLK_ENABLE();        //使能SPI3时钟

		//PB3.5
		GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_5;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz           
		GPIO_Initure.Alternate=GPIO_AF6_SPI3;           //PB3.5复用为SPI3
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}
}

/**
 * @brief	SPI3 读写一个字节
 *
 * @param   TxData	要写入的字节
 *
 * @return  u8		读取到的字节
 */
u8 SPI3_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI3_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}

/**
 * @brief	SPI3 写入一个字节
 *
 * @param   TxData	要写入的字节
 * @param   size	写入字节大小
 *
 * @return  u8		0:写入成功,其他:写入失败
 */
u8 SPI3_WriteByte(u8 *TxData,u16 size)
{
	return HAL_SPI_Transmit(&SPI3_Handler,TxData,size,1000);
}















