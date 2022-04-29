#include "key.h"
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
 *	KEY驱动代码
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
 * @brief	按键初始化函数
 *
 * @param   void
 *
 * @return  void
 */
void KEY_Init(void)
{
	/*
		KEY0 	- PD10
		KEY1 	- PD9
		KEY2 	- PD8
		WK_UP 	- PC13
	*/
	
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();           //开启GPIOD时钟

    GPIO_Initure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;	//PD8.9.10
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //输入
    GPIO_Initure.Pull = GPIO_PULLDOWN;      //下拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_13;         //PC13
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //输入
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
}

/**
 * @brief	按键处理函数
 *
 * @remark	注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
 *
 * @param   mode	0:不支持连续按,1:支持连续按
 *
 * @return  u8		返回按键值	
*					0:没有任何按键按下,1:KEY0按下,2:KEY1按下,3:KEY2按下,4:WK_UP按下
 */
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;   //按键松开标志

    if(mode == 1)key_up = 1; //支持连按

    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1))
    {
        delay_ms(10);
        key_up = 0;

        if(KEY0 == 0)       return KEY0_PRES;

        else if(KEY1 == 0)  return KEY1_PRES;

        else if(KEY2 == 0)  return KEY2_PRES;

        else if(WK_UP == 1) return WKUP_PRES;
    }

    else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)key_up = 1;

    return 0;   //无按键按下
}
