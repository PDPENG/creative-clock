#include "pwm.h"

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
 *	PWM驱动代码
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

TIM_HandleTypeDef TIM2_Handler;         //定时器2PWM句柄
TIM_OC_InitTypeDef TIM2_CHnHandler;	    //定时器2句柄


/**
 * @brief	TIM2 PWM输出初始化函数
 *
 * @param   arr		自动重装值
 * @param   psc		时钟预分频数
 *
 * @return  void
 */
void PWM_Init(u16 arr, u16 psc)
{
    TIM2_Handler.Instance = TIM2;          				//定时器2
    TIM2_Handler.Init.Prescaler = psc;     				//定时器分频
    TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; //向上计数模式
    TIM2_Handler.Init.Period = arr;        				//自动重装载值
    TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM2_Handler);       				//初始化PWM

    TIM2_CHnHandler.OCMode = TIM_OCMODE_PWM1; 			//模式选择PWM1
    TIM2_CHnHandler.Pulse = arr / 2;       				//设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM2_CHnHandler.OCPolarity = TIM_OCPOLARITY_HIGH; 	//输出比较极性为低

    HAL_TIM_PWM_ConfigChannel(&TIM2_Handler, &TIM2_CHnHandler, TIM_CHANNEL_1); //配置TIM2通道1
    HAL_TIM_PWM_Start(&TIM2_Handler, TIM_CHANNEL_1); 	//开启PWM通道1


    HAL_TIM_PWM_ConfigChannel(&TIM2_Handler, &TIM2_CHnHandler, TIM_CHANNEL_2); //配置TIM2通道2
    HAL_TIM_PWM_Start(&TIM2_Handler, TIM_CHANNEL_2); 	//开启PWM通道2
}


/**
 * @brief	定时器底层驱动，时钟使能，引脚配置，此函数会被HAL_TIM_PWM_Init()调用
 *
 * @param   htim		定时器句柄
 *
 * @return  void
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM2_CLK_ENABLE();				//使能定时器2
    __HAL_RCC_GPIOA_CLK_ENABLE();				//开启GPIOA时钟

    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1; //PA0.1
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;  		//复用推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;        	//上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   	//高速
    GPIO_Initure.Alternate = GPIO_AF1_TIM2;		//PA1复用为TIM2_CH1.CH2
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
}


/**
 * @brief	设置TIM2通道1的占空比
 *
 * @param   compare		比较值
 *
 * @return  void
 */
void TIM_SetTIM2Compare1(u32 compare)
{
    TIM2->CCR1 = compare;
}

/**
 * @brief	设置TIM2通道2的占空比
 *
 * @param   compare		比较值
 *
 * @return  void
 */
void TIM_SetTIM2Compare2(u32 compare)
{
    TIM2->CCR2 = compare;
}

