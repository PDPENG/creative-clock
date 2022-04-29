#include "rng.h"
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
 *	RNG(随机数发生器)驱动代码
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

RNG_HandleTypeDef RNG_Handler;  //RNG句柄

/**
 * @brief	初始化RNG函数
 *
 * @param   void
 *
 * @return  void
 */
u8 RNG_Init(void)
{
    u16 retry = 0;

    RNG_Handler.Instance = RNG;
    HAL_RNG_DeInit(&RNG_Handler);
    HAL_RNG_Init(&RNG_Handler);//初始化RNG

    while(__HAL_RNG_GET_FLAG(&RNG_Handler, RNG_FLAG_DRDY) == RESET && retry < 10000) //等待RNG准备就绪
    {
        retry++;
        delay_us(10);
    }

    if(retry >= 10000) return 1; //随机数产生器工作不正常

    return 0;
}

/**
 * @brief	RNG底层驱动，时钟配置与使能
 *
 * @param   hrng	RNG句柄
 *
 * @return  void
 */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    //RNG时钟配置为40Mhz    HSE/PLLM*PLLN/PLLQ(8Mhz/2*40/4 = 40Mhz)
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    //RNG时钟选择为PLL
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RNG;
    PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_PLL;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    __HAL_RCC_RNG_CLK_ENABLE();//使能RNG时钟
}

/**
 * @brief	得到随机数
 *
 * @param   void
 *
 * @return  u32		获取到的随机数
 */
u32 RNG_Get_RandomNum(void)
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler);
}

/**
 * @brief	生成[min,max]范围的随机数
 *
 * @param   min		最小值
 * @param   max		最大值
 *
 * @return  int		获取到的随机数
 */
int RNG_Get_RandomRange(int min, int max)
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler) % (max - min + 1) + min;
}




