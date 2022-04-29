#ifndef __RTC_H
#define __RTC_H

#include "sys.h"

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
 *	RTC驱动代码
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

extern RTC_HandleTypeDef RTC_Handler;  //RTC句柄
u8 RTC_Init(void);
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec, u8 ampm);   //RTC时间设置
HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date, u8 week);	//RTC日期设置
void RTC_Set_WakeUp(u32 wksel, u16 cnt);
void RTC_Set_AlarmA(u8 week, u8 hour, u8 min, u8 sec);


#endif


