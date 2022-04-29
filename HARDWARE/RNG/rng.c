#include "rng.h"
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
 *	RNG(�����������)��������
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

RNG_HandleTypeDef RNG_Handler;  //RNG���

/**
 * @brief	��ʼ��RNG����
 *
 * @param   void
 *
 * @return  void
 */
u8 RNG_Init(void)
{
    u16 retry = 0;

    RNG_Handler.Instance = RNG;
    HAL_RNG_DeInit(&RNG_Handler);
    HAL_RNG_Init(&RNG_Handler);//��ʼ��RNG

    while(__HAL_RNG_GET_FLAG(&RNG_Handler, RNG_FLAG_DRDY) == RESET && retry < 10000) //�ȴ�RNG׼������
    {
        retry++;
        delay_us(10);
    }

    if(retry >= 10000) return 1; //���������������������

    return 0;
}

/**
 * @brief	RNG�ײ�������ʱ��������ʹ��
 *
 * @param   hrng	RNG���
 *
 * @return  void
 */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    //RNGʱ������Ϊ40Mhz    HSE/PLLM*PLLN/PLLQ(8Mhz/2*40/4 = 40Mhz)
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    //RNGʱ��ѡ��ΪPLL
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RNG;
    PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_PLL;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    __HAL_RCC_RNG_CLK_ENABLE();//ʹ��RNGʱ��
}

/**
 * @brief	�õ������
 *
 * @param   void
 *
 * @return  u32		��ȡ���������
 */
u32 RNG_Get_RandomNum(void)
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler);
}

/**
 * @brief	����[min,max]��Χ�������
 *
 * @param   min		��Сֵ
 * @param   max		���ֵ
 *
 * @return  int		��ȡ���������
 */
int RNG_Get_RandomRange(int min, int max)
{
    return HAL_RNG_GetRandomNumber(&RNG_Handler) % (max - min + 1) + min;
}




