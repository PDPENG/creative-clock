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
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	�ⲿ�ж���������
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
 * @brief	�ⲿ�жϳ�ʼ������
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

    __HAL_RCC_GPIOC_CLK_ENABLE();               //����GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();               //����GPIODʱ��

    GPIO_Initure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;	//PD8.9.10
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;   //�½��ش���
    GPIO_Initure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_13;              //PC13
    GPIO_Initure.Mode = GPIO_MODE_IT_RISING;    //�����ش���
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    //�ж���8,9
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);     //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //ʹ���ж���0

    //�ж���10,13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 3); //��ռ���ȼ�Ϊ3�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���13
}

/**
 * @brief	EXTI9_5�жϷ�����
 *
 * @param   void
 *
 * @return  void
 */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);//�����жϴ����ú���

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);//�����жϴ����ú���
}



/**
 * @brief	EXTI15_10�жϷ�����
 *
 * @param   void
 *
 * @return  void
 */
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);//�����жϴ����ú���

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//�����жϴ����ú���
}



/**
 * @brief	�жϷ����������Ҫ��������,��HAL�������е��ⲿ�жϷ�����������ô˺���
 *
 * @param   GPIO_Pin	�ж����ź�
 *
 * @return  void
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(100);      //����

    switch(GPIO_Pin)
    {
        case  GPIO_PIN_13:		//���Ʒ�����״̬��ת
            BEEP_TogglePin;
            break;

        case  GPIO_PIN_8:		//����LED_B��״̬��ת
            LED_B_TogglePin;
            break;

        case  GPIO_PIN_9:		//����LED_G��״̬��ת
            LED_G_TogglePin;
            break;

        case  GPIO_PIN_10:		//����LED_R��״̬��ת
            LED_R_TogglePin;
            break;

        default:
            break;
    }
}



