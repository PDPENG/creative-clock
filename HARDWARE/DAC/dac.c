#include "dac.h"

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
 *	DAC驱动代码
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


DAC_HandleTypeDef DAC1_Handler;//DAC句柄

/**
 * @brief	初始化DAC
 *
 * @param   void
 *
 * @return  void
 */
void DAC1_Init(void)
{
    DAC_ChannelConfTypeDef DACCH1_Config;
    
    DAC1_Handler.Instance=DAC;
    HAL_DAC_Init(&DAC1_Handler); 	//初始化DAC
    
    DACCH1_Config.DAC_Trigger=DAC_TRIGGER_NONE;             			//不使用触发功能
    DACCH1_Config.DAC_OutputBuffer=DAC_OUTPUTBUFFER_DISABLE;			//DAC1输出缓冲关闭
	DACCH1_Config.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;		//DAC模式
	DACCH1_Config.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;//不连接到片内外设
    HAL_DAC_ConfigChannel(&DAC1_Handler,&DACCH1_Config,DAC_CHANNEL_1);	//DAC通道1配置
    
    HAL_DAC_Start(&DAC1_Handler,DAC_CHANNEL_1);  //开启DAC通道1
}


/**
 * @brief	DAC底层驱动，时钟配置，引脚 配置，此函数会被HAL_DAC_Init()调用
 *
 * @param   hdac	DAC句柄
 *
 * @return  void
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef GPIO_Initure;
	
    __HAL_RCC_DAC1_CLK_ENABLE();             //使能DAC时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_4;            //PA4
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

/**
 * @brief	设置通道1输出电压
 *
 * @param   vol		0~3300,代表0~3.3V
 *
 * @return  void
 */
void DAC1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
    HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);//12位右对齐数据格式设置DAC值
}
