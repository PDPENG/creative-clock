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
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	待机唤醒驱动代码
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

RTC_HandleTypeDef RTC_Handler;  //RTC句柄

/**
 * @brief	系统进入待机模式
 *
 * @param   void
 *
 * @return  void
 */
void Sys_Enter_Standby(void)
{
	__HAL_RCC_AHB2_FORCE_RESET();       //复位所有IO口
	
	__HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟
    __HAL_RCC_BACKUPRESET_FORCE();      //复位备份区域
    HAL_PWR_EnableBkUpAccess();         //后备区域访问使能  
	
	//STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置
	//RTC中断,再进入待机模式才可以正常唤醒,否则会有问题.	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&RTC_Handler);//关闭RTC写保护
    
    //关闭RTC相关中断，可能在RTC实验打开了
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&RTC_Handler,RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&RTC_Handler,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&RTC_Handler,RTC_IT_ALRA|RTC_IT_ALRB);
    
    //清除RTC相关中断标志位
    __HAL_RTC_ALARM_CLEAR_FLAG(&RTC_Handler,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&RTC_Handler,RTC_FLAG_TSF); 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler,RTC_FLAG_WUTF);
    
    __HAL_RCC_BACKUPRESET_RELEASE();                    //备份区域复位结束
    __HAL_RTC_WRITEPROTECTION_ENABLE(&RTC_Handler);     //使能RTC写保护
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //清除Wake_UP标志
	
		
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);           //设置WKUP用于唤醒
    HAL_PWR_EnterSTANDBYMode();                         //进入待机模式     
}
 
/**
 * @brief	检测WKUP脚的信号
 *
 * @param   void
 *
 * @return  u8		1:连续按下3s以上
 *					0:错误的触发
 */	
u8 Check_WKUP(void) 
{
	u8 t=0;
	u8 tx=0;//记录松开的次数
	LED_B(0); //亮灯DS0 
	while(1)
	{
		if(WKUP_KD)//已经按下了
		{
			t++;
			tx=0;
		}else 
		{
			tx++; 
			if(tx>3)//超过90ms内没有WKUP信号
			{
				LED_B(1);
				return 0;//错误的按键,按下次数不够
			}
		}
		delay_ms(30);
		if(t>=100)//按下超过3秒钟
		{
			LED_B(0); //点亮DS0 
			return 1; //按下3s以上了
		}
	}
}  
/**
 * @brief	外部中断线0中断服务函数
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
 * @brief	中断线15-13中断处理过程,此函数会被HAL_GPIO_EXTI_IRQHandler()调用
 *
 * @param   GPIO_Pin	引脚
 *
 * @return  void
 */	
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==GPIO_PIN_13)//PC13
    {
        if(Check_WKUP())//关机
        {
            Sys_Enter_Standby();//进入待机模式
        }
    }   
		
}
/**
 * @brief	PC13 WKUP唤醒初始化
 *
 * @param   void
 *
 * @return  void
 */	
void WKUP_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	
    GPIO_Initure.Pin=GPIO_PIN_13;           //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;  //中断,上升沿
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
	HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_C, PWR_GPIO_BIT_13);
	HAL_PWREx_EnablePullUpPullDownConfig();
	
    //检查是否是正常开机
    if(Check_WKUP()==0)
    {
        Sys_Enter_Standby();//不是开机，进入待机模式
    }

    HAL_NVIC_SetPriority(EXTI15_10_IRQn,0x02,0x02);//抢占优先级2，子优先级2
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
