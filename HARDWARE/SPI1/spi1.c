#include "spi1.h"
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
 *	SPI1驱动代码
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

SPI_HandleTypeDef SPI1_Handler;  //SPI1句柄

/**
 * @brief	SPI1初始化代码，配置成主机模式
 *
 * @param   void
 *
 * @return  void
 */
void SPI1_Init(void)
{
    SPI1_Handler.Instance = SPI1;                       //SPI1
    SPI1_Handler.Init.Mode = SPI_MODE_MASTER;           //设置SPI工作模式，设置为主模式
    SPI1_Handler.Init.Direction = SPI_DIRECTION_2LINES; //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI1_Handler.Init.DataSize = SPI_DATASIZE_8BIT;     //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI1_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;  //串行同步时钟的空闲状态为高电平
    SPI1_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;       //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI1_Handler.Init.NSS = SPI_NSS_SOFT;               //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI1_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; //定义波特率预分频的值:波特率预分频值为256
    SPI1_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;      //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI1_Handler.Init.TIMode = SPI_TIMODE_DISABLE;      //关闭TI模式
    SPI1_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; //关闭硬件CRC校验
    SPI1_Handler.Init.CRCPolynomial = 7;                //CRC值计算的多项式
    HAL_SPI_Init(&SPI1_Handler);//初始化SPI1

    __HAL_SPI_ENABLE(&SPI1_Handler);                    //使能SPI1

    SPI1_WriteByte(0Xff);                           //启动传输

}


/**
 * @brief	SPI1底层驱动，时钟使能，引脚配置
 * 			注意:SPI1底层驱动初始化会在SPI3底层驱动初始化中一起调用！！!(详见SPI3.C)
 *
 * @param   hspi	SPI句柄
 *
 * @return  void
 */
//void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
//{
//	if( hspi->Instance == SPI1 )
//	{
//		GPIO_InitTypeDef GPIO_Initure;
//
//		__HAL_RCC_GPIOA_CLK_ENABLE();       //使能GPIOA时钟
//		__HAL_RCC_SPI1_CLK_ENABLE();        //使能SPI1时钟

//		//PA5.6.7
//		GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
//		GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
//		GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
//		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz
//		GPIO_Initure.Alternate=GPIO_AF5_SPI1;           //复用为SPI1
//		HAL_GPIO_Init(GPIOA,&GPIO_Initure);
//	}
//}

/**
 * @brief	SPI1速度设置函数
 *
 * @param   SPI_BaudRate_Prescaler	SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 *
 * @return  void
 */
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI1_Handler);            //关闭SPI
    SPI1_Handler.Instance->CR1 &= 0XFFC7;        //位3-5清零，用来设置波特率
    SPI1_Handler.Instance->CR1 |= SPI_BaudRatePrescaler; //设置SPI速度
    __HAL_SPI_ENABLE(&SPI1_Handler);             //使能SPI

}

/**
 * @brief	SPI1 读写一个字节
 *
 * @param   TxData		要写入的字节
 *
 * @return  u8 			读取到的字节
 */
u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI1_Handler, &TxData, &Rxdata, 1, 1000);
    return Rxdata;          		    //返回收到的数据
}

/**
 * @brief	SPI1 写入一个字节
 *
 * @param   TxData	要写入的字节
 *
 * @return  u8		0:写入成功,其他:写入失败
 */
u8 SPI1_WriteByte(u8 TxData)
{
    return HAL_SPI_Transmit(&SPI1_Handler, &TxData, 1, 1000);
}


