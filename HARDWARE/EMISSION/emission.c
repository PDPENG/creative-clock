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
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	红外发射驱动代码
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
 * @brief	红外发射初始化函数
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
 * @brief	IO口模拟38KHz
 *
 * @param   us		持续多少微妙的时间
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
 * @brief	将一个字节数据进行位置变换
 *
 * @param   u8		需要变换的数据
 *
 * @return  u8		得到变换后的数据
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
 * @brief	发送红外数据函数
 *
 * @param   addrcode	地址码
 * @param   keycode		控制码
 * @param   times		控制码连发次数
 *
 * @return  void
 */
void Infrared_Send_Data(u8 addrcode, u8 keycode, u8 times)
{
    u8 index;
    u32 data;

    keycode = m2m(keycode);

    data = ((~keycode & 0xFF) << 24) + ((keycode & 0xFF) << 16) + ((~addrcode & 0xff) << 8) + addrcode;
    //32位
    EMISSION_PIN(0);
    IR38KHz_us(9000);
    EMISSION_PIN(1);
    delay_us(4500);
    //引导码
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

    //连发码
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




