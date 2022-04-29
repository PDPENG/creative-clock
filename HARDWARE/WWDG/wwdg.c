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
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	���ڿ��Ź���������
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


WWDG_HandleTypeDef WWDG_Handler;     //���ڿ��Ź����

/**
 * @brief	��ʼ�����ڿ��Ź� 
 *			Fwwdg=PCLK1/(4096*2^fprer).PCLK1ΪAPB1ʱ��:80Mhz
 *
 * @param   tr   	T[6:0],������ֵ
 * @param   wr   	W[6:0],����ֵ 
 * @param   fprer   ��Ƶϵ����WDGTB��,�����2λ��Ч
 *
 * @return  void
 */
void WWDG_Init(u8 tr,u8 wr,u32 fprer)
{
    WWDG_Handler.Instance=WWDG;
    WWDG_Handler.Init.Prescaler=fprer;   //���÷�Ƶϵ��
    WWDG_Handler.Init.Window=wr;         //���ô���ֵ
    WWDG_Handler.Init.Counter=tr;        //���ü�����ֵ
	WWDG_Handler.Init.EWIMode   = WWDG_EWI_DISABLE;
    HAL_WWDG_Init(&WWDG_Handler);        //��ʼ��WWDG
	
	__HAL_WWDG_ENABLE_IT(&WWDG_Handler, WWDG_IT_EWI);	//���������ж�
}
/**
 * @brief	WWDG�ײ�������ʱ�����ã��ж�����
 *			�˺����ᱻHAL_WWDG_Init()����
 *
 * @param   hwwdg	���ڿ��Ź����
 *
 * @return  void
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{   
    __HAL_RCC_WWDG_CLK_ENABLE();    		//ʹ�ܴ��ڿ��Ź�ʱ��
        
    HAL_NVIC_SetPriority(WWDG_IRQn,2,3);    //��ռ���ȼ�2�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(WWDG_IRQn);          //ʹ�ܴ��ڿ��Ź��ж�
}


/**
 * @brief	���ڿ��Ź��жϷ�����
 *
 * @param   void
 *
 * @return  void
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&WWDG_Handler);//����WWDG�����жϴ�����
}

/**
 * @brief	�жϷ������������
 *			�˺����ᱻHAL_WWDG_IRQHandler()����
 *
 * @param   hwwdg	���ڿ��Ź����
 *
 * @return  void
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
    HAL_WWDG_Refresh(&WWDG_Handler);//���´��ڿ��Ź�ֵ
	LED_B_TogglePin;
}
