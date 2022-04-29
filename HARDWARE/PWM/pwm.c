#include "pwm.h"

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
 *	PWM��������
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

TIM_HandleTypeDef TIM2_Handler;         //��ʱ��2PWM���
TIM_OC_InitTypeDef TIM2_CHnHandler;	    //��ʱ��2���


/**
 * @brief	TIM2 PWM�����ʼ������
 *
 * @param   arr		�Զ���װֵ
 * @param   psc		ʱ��Ԥ��Ƶ��
 *
 * @return  void
 */
void PWM_Init(u16 arr, u16 psc)
{
    TIM2_Handler.Instance = TIM2;          				//��ʱ��2
    TIM2_Handler.Init.Prescaler = psc;     				//��ʱ����Ƶ
    TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; //���ϼ���ģʽ
    TIM2_Handler.Init.Period = arr;        				//�Զ���װ��ֵ
    TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM2_Handler);       				//��ʼ��PWM

    TIM2_CHnHandler.OCMode = TIM_OCMODE_PWM1; 			//ģʽѡ��PWM1
    TIM2_CHnHandler.Pulse = arr / 2;       				//���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM2_CHnHandler.OCPolarity = TIM_OCPOLARITY_HIGH; 	//����Ƚϼ���Ϊ��

    HAL_TIM_PWM_ConfigChannel(&TIM2_Handler, &TIM2_CHnHandler, TIM_CHANNEL_1); //����TIM2ͨ��1
    HAL_TIM_PWM_Start(&TIM2_Handler, TIM_CHANNEL_1); 	//����PWMͨ��1


    HAL_TIM_PWM_ConfigChannel(&TIM2_Handler, &TIM2_CHnHandler, TIM_CHANNEL_2); //����TIM2ͨ��2
    HAL_TIM_PWM_Start(&TIM2_Handler, TIM_CHANNEL_2); 	//����PWMͨ��2
}


/**
 * @brief	��ʱ���ײ�������ʱ��ʹ�ܣ��������ã��˺����ᱻHAL_TIM_PWM_Init()����
 *
 * @param   htim		��ʱ�����
 *
 * @return  void
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM2_CLK_ENABLE();				//ʹ�ܶ�ʱ��2
    __HAL_RCC_GPIOA_CLK_ENABLE();				//����GPIOAʱ��

    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1; //PA0.1
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;  		//�����������
    GPIO_Initure.Pull = GPIO_PULLUP;        	//����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   	//����
    GPIO_Initure.Alternate = GPIO_AF1_TIM2;		//PA1����ΪTIM2_CH1.CH2
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
}


/**
 * @brief	����TIM2ͨ��1��ռ�ձ�
 *
 * @param   compare		�Ƚ�ֵ
 *
 * @return  void
 */
void TIM_SetTIM2Compare1(u32 compare)
{
    TIM2->CCR1 = compare;
}

/**
 * @brief	����TIM2ͨ��2��ռ�ձ�
 *
 * @param   compare		�Ƚ�ֵ
 *
 * @return  void
 */
void TIM_SetTIM2Compare2(u32 compare)
{
    TIM2->CCR2 = compare;
}

