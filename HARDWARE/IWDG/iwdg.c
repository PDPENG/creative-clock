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
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	�������Ź���������
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

IWDG_HandleTypeDef IWDG_Handler; //�������Ź����

/**
 * @brief	��ʼ���������Ź�����
 *			ʱ�����(���):Tout=((2^prer)*rlr)/32 (ms)
 *
 * @param   prer	��Ƶ��:IWDG_PRESCALER_4~IWDG_PRESCALER_256
 * @param   rlr		�Զ���װ��ֵ,0~0XFFF
 *
 * @return  void
 */
void IWDG_Init(u8 prer, u16 rlr)
{
    IWDG_Handler.Instance = IWDG;
    IWDG_Handler.Init.Prescaler = prer;					//����IWDG��Ƶϵ��
    IWDG_Handler.Init.Reload = rlr;						//��װ��ֵ
    IWDG_Handler.Init.Window = IWDG_WINDOW_DISABLE;
    HAL_IWDG_Init(&IWDG_Handler);						//��ʼ��IWDG�������������Ź�
}

/**
 * @brief	ι�������Ź�
 *
 * @param   void
 *
 * @return  void
 */
void IWDG_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_Handler); 	//ι��
}
