#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "beep.h"

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
 *	外部中断驱动代码
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

/**
 * @brief	外部中断初始化函数
 *
 * @param   void
 *
 * @return  void
 */
void EXTI_Init(void)
{
    /*
    	KEY0 	- PD10
    	KEY1 	- PD9
    	KEY2 	- PD8
    	WK_UP 	- PC13
    */
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();               //开启GPIOD时钟

    GPIO_Initure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;	//PD8.9.10
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;   //下降沿触发
    GPIO_Initure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_13;              //PC13
    GPIO_Initure.Mode = GPIO_MODE_IT_RISING;    //上升沿触发
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    //中断线8,9
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);     //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //使能中断线0

    //中断线10,13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 3); //抢占优先级为3，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线13
}

/**
 * @brief	EXTI9_5中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);//调用中断处理公用函数

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);//调用中断处理公用函数
}



/**
 * @brief	EXTI15_10中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);//调用中断处理公用函数

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//调用中断处理公用函数
}



/**
 * @brief	中断服务程序中需要做的事情,在HAL库中所有的外部中断服务函数都会调用此函数
 *
 * @param   GPIO_Pin	中断引脚号
 *
 * @return  void
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(100);      //消抖

    switch(GPIO_Pin)
    {
        case  GPIO_PIN_13:		//控制蜂鸣器状态翻转
            BEEP_TogglePin;
            break;

        case  GPIO_PIN_8:		//控制LED_B灯状态翻转
            LED_B_TogglePin;
            break;

        case  GPIO_PIN_9:		//控制LED_G灯状态翻转
            LED_G_TogglePin;
            break;

        case  GPIO_PIN_10:		//控制LED_R灯状态翻转
            LED_R_TogglePin;
            break;

        default:
            break;
    }
}



