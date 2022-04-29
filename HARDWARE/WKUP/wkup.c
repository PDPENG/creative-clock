#include "wkup.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	����������������
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/

RTC_HandleTypeDef RTC_Handler;  //RTC���

/**
 * @brief	ϵͳ�������ģʽ
 *
 * @param   void
 *
 * @return  void
 */
void Sys_Enter_Standby(void)
{
	__HAL_RCC_AHB2_FORCE_RESET();       //��λ����IO��
	
	__HAL_RCC_PWR_CLK_ENABLE();         //ʹ��PWRʱ��
    __HAL_RCC_BACKUPRESET_FORCE();      //��λ��������
    HAL_PWR_EnableBkUpAccess();         //���������ʹ��  
	
	//STM32F4,��������RTC����жϺ�,�����ȹر�RTC�ж�,�����жϱ�־λ,Ȼ����������
	//RTC�ж�,�ٽ������ģʽ�ſ�����������,�����������.	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&RTC_Handler);//�ر�RTCд����
    
    //�ر�RTC����жϣ�������RTCʵ�����
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&RTC_Handler,RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&RTC_Handler,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&RTC_Handler,RTC_IT_ALRA|RTC_IT_ALRB);
    
    //���RTC����жϱ�־λ
    __HAL_RTC_ALARM_CLEAR_FLAG(&RTC_Handler,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&RTC_Handler,RTC_FLAG_TSF); 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler,RTC_FLAG_WUTF);
    
    __HAL_RCC_BACKUPRESET_RELEASE();                    //��������λ����
    __HAL_RTC_WRITEPROTECTION_ENABLE(&RTC_Handler);     //ʹ��RTCд����
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //���Wake_UP��־
	
		
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);           //����WKUP���ڻ���
    HAL_PWR_EnterSTANDBYMode();                         //�������ģʽ     
}
 
/**
 * @brief	���WKUP�ŵ��ź�
 *
 * @param   void
 *
 * @return  u8		1:��������3s����
 *					0:����Ĵ���
 */	
u8 Check_WKUP(void) 
{
	u8 t=0;
	u8 tx=0;//��¼�ɿ��Ĵ���
	LED_B(0); //����DS0 
	while(1)
	{
		if(WKUP_KD)//�Ѿ�������
		{
			t++;
			tx=0;
		}else 
		{
			tx++; 
			if(tx>3)//����90ms��û��WKUP�ź�
			{
				LED_B(1);
				return 0;//����İ���,���´�������
			}
		}
		delay_ms(30);
		if(t>=100)//���³���3����
		{
			LED_B(0); //����DS0 
			return 1; //����3s������
		}
	}
}  
/**
 * @brief	�ⲿ�ж���0�жϷ�����
 *
 * @param   void
 *
 * @return  void
 */	
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/**
 * @brief	�ж���15-13�жϴ�������,�˺����ᱻHAL_GPIO_EXTI_IRQHandler()����
 *
 * @param   GPIO_Pin	����
 *
 * @return  void
 */	
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_13)//PC13
    {
        if(Check_WKUP())//�ػ�
        {
            Sys_Enter_Standby();//�������ģʽ
        }
    }   
		
}
/**
 * @brief	PC13 WKUP���ѳ�ʼ��
 *
 * @param   void
 *
 * @return  void
 */	
void WKUP_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_13;           //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;  //�ж�,������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
	HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_C, PWR_GPIO_BIT_13);
	HAL_PWREx_EnablePullUpPullDownConfig();
	
    //����Ƿ�����������
    if(Check_WKUP()==0)
    {
        Sys_Enter_Standby();//���ǿ������������ģʽ
    }

    HAL_NVIC_SetPriority(EXTI15_10_IRQn,0x02,0x02);//��ռ���ȼ�2�������ȼ�2
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}