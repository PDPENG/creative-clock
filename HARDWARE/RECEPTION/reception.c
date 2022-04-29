#include "reception.h"
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
 *	������ս�����������
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

TIM_HandleTypeDef TIM3_Handler;      //��ʱ��1���


/**
 * @brief	����ң�س�ʼ������������IO�Լ�TIM3_CH4�����벶��
 *			TIM3����APB2��80Mhz
 *
 * @param   void
 *
 * @return  void
 */
void Reception_Init(void)
{
    TIM_IC_InitTypeDef TIM3_CH4Config;

    TIM3_Handler.Instance = TIM3;						//ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler = 79;					//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; //���ϼ�����
    TIM3_Handler.Init.Period = 10000;                   //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM3_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_IC_Init(&TIM3_Handler);

    //��ʼ��TIM3���벶�����
    TIM3_CH4Config.ICPolarity = TIM_ICPOLARITY_RISING;  	//�����ز���
    TIM3_CH4Config.ICSelection = TIM_ICSELECTION_DIRECTTI; 	//ӳ�䵽TI1��
    TIM3_CH4Config.ICPrescaler = TIM_ICPSC_DIV1;        	//���������Ƶ������Ƶ
    TIM3_CH4Config.ICFilter = 0x03;                     	//IC1F=0003 8����ʱ��ʱ�������˲�
    HAL_TIM_IC_ConfigChannel(&TIM3_Handler, &TIM3_CH4Config, TIM_CHANNEL_4); //����TIM3ͨ��4
    HAL_TIM_IC_Start_IT(&TIM3_Handler, TIM_CHANNEL_4);  	//��ʼ����TIM3��ͨ��4
    __HAL_TIM_ENABLE_IT(&TIM3_Handler, TIM_IT_UPDATE);  	//ʹ�ܸ����ж�
}


/**
 * @brief	TIM3�ײ�������ʱ��ʹ�ܣ���������
 *			�˺����ᱻHAL_TIM_IC_Init()����
 *
 * @param   htim	��ʱ�����
 *
 * @return  void
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��

    GPIO_Initure.Pin = GPIO_PIN_1;          //PB1
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;    //�����������
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    GPIO_Initure.Alternate = GPIO_AF2_TIM3; //PB1����ΪTIM3ͨ��4
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}


//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����
//[4]:����������Ƿ��Ѿ�������
//[3:0]:�����ʱ��
u8 	RmtSta = 0;
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec = 0;	//������յ�������
u8  RmtCnt = 0;	//�������µĴ���


/**
 * @brief	TIM3�жϷ�����
 *
 * @param   void
 *
 * @return  void
 */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

/**
 * @brief	TIM3����(���)�жϻص�����
 *
 * @param   htim	��ʱ�����
 *
 * @return  void
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        if(RmtSta & 0x80) //�ϴ������ݱ����յ���
        {
            RmtSta &= ~0X10;						//ȡ���������Ѿ���������

            if((RmtSta & 0X0F) == 0X00)RmtSta |= 1 << 6; //����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�

            if((RmtSta & 0X0F) < 14)RmtSta++;

            else
            {
                RmtSta &= ~(1 << 7); //���������ʶ
                RmtSta &= 0XF0;	//��ռ�����
            }
        }
    }
}

/**
 * @brief	��ʱ�����벶���жϻص�����
 *
 * @param   htim	��ʱ�����
 *
 * @return  void
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
    if(htim->Instance == TIM3)
    {
        if(RDATA)//�����ز���
        {
            TIM_RESET_CAPTUREPOLARITY(&TIM3_Handler, TIM_CHANNEL_4);  //һ��Ҫ�����ԭ�������ã���
            TIM_SET_CAPTUREPOLARITY(&TIM3_Handler, TIM_CHANNEL_4, TIM_ICPOLARITY_FALLING); //CC1P=1 ����Ϊ�½��ز���
            __HAL_TIM_SET_COUNTER(&TIM3_Handler, 0); //��ն�ʱ��ֵ
            RmtSta |= 0X10;					//����������Ѿ�������
        }

        else  //�½��ز���
        {
            Dval = HAL_TIM_ReadCapturedValue(&TIM3_Handler, TIM_CHANNEL_4); //��ȡCCR1Ҳ������CC1IF��־λ
            TIM_RESET_CAPTUREPOLARITY(&TIM3_Handler, TIM_CHANNEL_4);  //һ��Ҫ�����ԭ�������ã���
            TIM_SET_CAPTUREPOLARITY(&TIM3_Handler, TIM_CHANNEL_4, TIM_ICPOLARITY_RISING); //����TIM5ͨ��1�����ز���

            if(RmtSta & 0X10)					//���һ�θߵ�ƽ����
            {
                if(RmtSta & 0X80) //���յ���������
                {

                    if(Dval > 300 && Dval < 800)			//560Ϊ��׼ֵ,560us
                    {
                        RmtRec <<= 1;	//����һλ.
                        RmtRec |= 0;	//���յ�0
                    }

                    else if(Dval > 1400 && Dval < 1800)	//1680Ϊ��׼ֵ,1680us
                    {
                        RmtRec <<= 1;	//����һλ.
                        RmtRec |= 1;	//���յ�1
                    }

                    else if(Dval > 2100 && Dval < 2800)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
                    {
                        RmtCnt++; 		//������������1��
                        RmtSta &= 0XF0;	//��ռ�ʱ��
                    }
                }

                else if(Dval > 4200 && Dval < 4800)		//4500Ϊ��׼ֵ4.5ms
                {
                    RmtSta |= 1 << 7;	//��ǳɹ����յ���������
                    RmtCnt = 0;		//�����������������
                }
            }

            RmtSta &= ~(1 << 4);
        }
    }
}

/**
 * @brief	����������
 *
 * @param   void
 *
 * @return  u8		0,û���κΰ�������
 *					����,���µİ�����ֵ
 */
u8 Remote_Scan(void)
{
    u8 sta = 0;
    u8 t1, t2;

    if(RmtSta & (1 << 6)) //�õ�һ��������������Ϣ��
    {
        t1 = RmtRec >> 24;			//�õ���ַ��
        t2 = (RmtRec >> 16) & 0xff;	//�õ���ַ����

        if((t1 == (u8)~t2) && t1 == REMOTE_ID) //����ң��ʶ����(ID)����ַ
        {
            t1 = RmtRec >> 8;
            t2 = RmtRec;

            if(t1 == (u8)~t2)sta = t1; //��ֵ��ȷ
        }

        if((sta == 0) || ((RmtSta & 0X80) == 0)) //�������ݴ���/ң���Ѿ�û�а�����
        {
            RmtSta &= ~(1 << 6); //������յ���Ч������ʶ
            RmtCnt = 0;		//�����������������
        }
    }

    return sta;
}
