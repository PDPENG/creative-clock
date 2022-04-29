#include "sys.h"
#include "iwdg.h"

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
 *	独立看门狗驱动代码
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

IWDG_HandleTypeDef IWDG_Handler; //独立看门狗句柄

/**
 * @brief	初始化独立看门狗函数
 *			时间计算(大概):Tout=((2^prer)*rlr)/32 (ms)
 *
 * @param   prer	分频数:IWDG_PRESCALER_4~IWDG_PRESCALER_256
 * @param   rlr		自动重装载值,0~0XFFF
 *
 * @return  void
 */
void IWDG_Init(u8 prer, u16 rlr)
{
    IWDG_Handler.Instance = IWDG;
    IWDG_Handler.Init.Prescaler = prer;					//设置IWDG分频系数
    IWDG_Handler.Init.Reload = rlr;						//重装载值
    IWDG_Handler.Init.Window = IWDG_WINDOW_DISABLE;
    HAL_IWDG_Init(&IWDG_Handler);						//初始化IWDG并开启独立看门狗
}

/**
 * @brief	喂独立看门狗
 *
 * @param   void
 *
 * @return  void
 */
void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_Handler); 	//喂狗
}
