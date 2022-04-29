#include "wwdg.h"
#include "led.h"
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
 *	窗口看门狗驱动代码
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


WWDG_HandleTypeDef WWDG_Handler;     //窗口看门狗句柄

/**
 * @brief	初始化窗口看门狗 
 *			Fwwdg=PCLK1/(4096*2^fprer).PCLK1为APB1时钟:80Mhz
 *
 * @param   tr   	T[6:0],计数器值
 * @param   wr   	W[6:0],窗口值 
 * @param   fprer   分频系数（WDGTB）,仅最低2位有效
 *
 * @return  void
 */
void WWDG_Init(u8 tr,u8 wr,u32 fprer)
{
    WWDG_Handler.Instance=WWDG;
    WWDG_Handler.Init.Prescaler=fprer;   //设置分频系数
    WWDG_Handler.Init.Window=wr;         //设置窗口值
    WWDG_Handler.Init.Counter=tr;        //设置计数器值
	WWDG_Handler.Init.EWIMode   = WWDG_EWI_DISABLE;
    HAL_WWDG_Init(&WWDG_Handler);        //初始化WWDG
	
	__HAL_WWDG_ENABLE_IT(&WWDG_Handler, WWDG_IT_EWI);	//开启唤醒中断
}
/**
 * @brief	WWDG底层驱动，时钟配置，中断配置
 *			此函数会被HAL_WWDG_Init()调用
 *
 * @param   hwwdg	窗口看门狗句柄
 *
 * @return  void
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{   
    __HAL_RCC_WWDG_CLK_ENABLE();    		//使能窗口看门狗时钟
        
    HAL_NVIC_SetPriority(WWDG_IRQn,2,3);    //抢占优先级2，子优先级为3
    HAL_NVIC_EnableIRQ(WWDG_IRQn);          //使能窗口看门狗中断
}


/**
 * @brief	窗口看门狗中断服务函数
 *
 * @param   void
 *
 * @return  void
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&WWDG_Handler);//调用WWDG共用中断处理函数
}

/**
 * @brief	中断服务函数处理过程
 *			此函数会被HAL_WWDG_IRQHandler()调用
 *
 * @param   hwwdg	窗口看门狗句柄
 *
 * @return  void
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
    HAL_WWDG_Refresh(&WWDG_Handler);//更新窗口看门狗值
	LED_B_TogglePin;
}
