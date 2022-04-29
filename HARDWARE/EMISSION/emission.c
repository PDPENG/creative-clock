#include "emission.h"
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
 *	���ⷢ����������
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
 * @brief	���ⷢ���ʼ������
 *
 * @param   void
 *
 * @return  void
 */
void Emission_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}


/**
 * @brief	IO��ģ��38KHz
 *
 * @param   us		��������΢���ʱ��
 *
 * @return  void
 */
void IR38KHz_us(u32 us)
{
    u32 cnt;

    cnt = us / 24;

    for(u32 i = 0; i < cnt; i++)
    {
        EMISSION_PIN(0);
        delay_us(6);
        EMISSION_PIN(1);
        delay_us(18);
    }
}

/**
 * @brief	��һ���ֽ����ݽ���λ�ñ任
 *
 * @param   u8		��Ҫ�任������
 *
 * @return  u8		�õ��任�������
 */
u8 m2m(u8 data)
{
    u8 y = 0;

    for(u8 i = 0; i < 8; i++)
    {
        if(((data >> i) & 0x01) == 1)
        {
            y |= (0x01 << (7 - i));
        }
    }

    return y;
}

/**
 * @brief	���ͺ������ݺ���
 *
 * @param   addrcode	��ַ��
 * @param   keycode		������
 * @param   times		��������������
 *
 * @return  void
 */
void Infrared_Send_Data(u8 addrcode, u8 keycode, u8 times)
{
    u8 index;
    u32 data;

    keycode = m2m(keycode);

    data = ((~keycode & 0xFF) << 24) + ((keycode & 0xFF) << 16) + ((~addrcode & 0xff) << 8) + addrcode;
    //32λ
    EMISSION_PIN(0);
    IR38KHz_us(9000);
    EMISSION_PIN(1);
    delay_us(4500);
    //������
    EMISSION_PIN(0);

    for(index = 0; index < 32; index++)
    {
        IR38KHz_us(560);
        EMISSION_PIN(1);

        if(((data >> index) & 0x00000001) == 0)
        {
            delay_us(550);
        }

        else
        {
            delay_us(1680);
        }

        EMISSION_PIN(0);
    }

    IR38KHz_us(560);
    EMISSION_PIN(1);

    //������
    for(index = 0; index < times; index++)
    {
        EMISSION_PIN(0);
        IR38KHz_us(9000);
        EMISSION_PIN(1);
        delay_us(2250);
        EMISSION_PIN(0);
        IR38KHz_us(560);
        EMISSION_PIN(1);
        delay_us(97940);
    }

}




