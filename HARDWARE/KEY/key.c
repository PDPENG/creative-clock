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
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	KEY��������
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/

/**
 * @brief	������ʼ������
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

    __HAL_RCC_GPIOC_CLK_ENABLE();           //����GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();           //����GPIODʱ��

    GPIO_Initure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;	//PD8.9.10
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //����
    GPIO_Initure.Pull = GPIO_PULLDOWN;      //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_13;         //PC13
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //����
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
}

/**
 * @brief	����������
 *
 * @remark	ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
 *
 * @param   mode	0:��֧��������,1:֧��������
 *
 * @return  u8		���ذ���ֵ	
*					0:û���κΰ�������,1:KEY0����,2:KEY1����,3:KEY2����,4:WK_UP����
 */
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;   //�����ɿ���־

    if(mode == 1)key_up = 1; //֧������

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

    return 0;   //�ް�������
}
