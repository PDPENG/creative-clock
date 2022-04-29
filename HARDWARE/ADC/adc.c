#include "adc.h"
#include "delay.h"
#include "lcd.h"

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
 *	ADC��������
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

ADC_HandleTypeDef ADC1_Handler;//ADC���

/**
 * @brief	��ʼ��ADC����
 *
 * @param   void
 *
 * @return  void
 */
void MY_ADC_Init(void)
{
    ADC1_Handler.Instance = ADC1;
    ADC1_Handler.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; //1��Ƶ��ADCCLK=SYSCLK/4=80/4=20MHZ	ADCʱ�����Ϊ36Mhz
    ADC1_Handler.Init.Resolution = ADC_RESOLUTION_12B;           //12λģʽ
    ADC1_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;           //�Ҷ���
    ADC1_Handler.Init.ScanConvMode = DISABLE;                    //��ɨ��ģʽ
    ADC1_Handler.Init.EOCSelection = DISABLE;                    //�ر�EOC�ж�
    ADC1_Handler.Init.ContinuousConvMode = DISABLE;              //�ر�����ת��
    ADC1_Handler.Init.NbrOfConversion = 1;                       //1��ת���ڹ��������� Ҳ����ֻת����������1
    ADC1_Handler.Init.DiscontinuousConvMode = DISABLE;           //��ֹ����������ģʽ
    ADC1_Handler.Init.NbrOfDiscConversion = 0;                   //����������ͨ����Ϊ0
    ADC1_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;     //�������
    ADC1_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; //ʹ���������

    ADC1_Handler.Init.LowPowerAutoWait = DISABLE;
    ADC1_Handler.Init.DMAContinuousRequests = DISABLE;
    ADC1_Handler.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    ADC1_Handler.Init.OversamplingMode = DISABLE;

    ADC1_Handler.Init.DMAContinuousRequests = DISABLE;           //�ر�DMA����
    HAL_ADC_Init(&ADC1_Handler);                                 //��ʼ��
}
/**
 * @brief	ADC�ײ��������������ã�ʱ��ʹ�ܣ��˺����ᱻHAL_ADC_Init()����
 *
 * @param   hadc	ADC���
 *
 * @return  void
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    GPIO_InitTypeDef GPIO_Initure;

    //ѡ��ADCʱ��ԴΪSYSCLK(80Mhz)
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    __HAL_RCC_ADC_CLK_ENABLE();            //ʹ��ADC1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��

    GPIO_Initure.Pin = GPIO_PIN_4;          //PA4
    GPIO_Initure.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;   //ģ��
    GPIO_Initure.Pull = GPIO_NOPULL;        //����������
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
}
/**
 * @brief	���ADCֵ
 *
 * @param   ch		ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
 *
 * @return  u16		ת�����
 */
u16 Get_Adc(u32 ch)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;

    ADC1_ChanConf.Channel = ch;                                 //ͨ��
    ADC1_ChanConf.Rank = ADC_REGULAR_RANK_1;						//��1�����У�����1
    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;      //����ʱ��
    ADC1_ChanConf.SingleDiff = ADC_SINGLE_ENDED;				//ADC��������
    ADC1_ChanConf.OffsetNumber = ADC_OFFSET_NONE;				//ƫ�ƺ�ѡ��
    ADC1_ChanConf.Offset = 0;
    HAL_ADC_ConfigChannel(&ADC1_Handler, &ADC1_ChanConf);       //ͨ������

    HAL_ADC_Start(&ADC1_Handler);                               //����ADC

    HAL_ADC_PollForConversion(&ADC1_Handler, 10);               //��ѯת��

    return (u16)HAL_ADC_GetValue(&ADC1_Handler);	        //�������һ��ADC1�������ת�����
}

/**
 * @brief	��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ��
 *
 * @param   ch		ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
 * @param   times	��ȡ����
 *
 * @return  u16		ͨ��ch��times��ת�����ƽ��ֵ
 */
u16 Get_Adc_Average(u32 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;

    for(t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
        delay_ms(5);
    }

    return temp_val / times;
}
/**
 * @brief	�õ��¶�ֵ
 *
 * @param   void
 *
 * @return  short	�¶�ֵ(������100��,��λ:��.)
 */
short Get_Temprate(void)
{
    u32 adcx;
    short result;
    double temperate;
    adcx = Get_Adc_Average(ADC_CHANNEL_TEMPSENSOR, 10); //��ȡ�ڲ��¶ȴ�����ͨ��,10��ȡƽ��

    temperate = (float)adcx * (3.3 / 4096);		//��ѹֵ
    temperate = (0.76 - temperate) / 0.0025 + 25; //ת��Ϊ�¶�ֵ
    result = temperate *= 100;					//����100��.
    return result;
}



