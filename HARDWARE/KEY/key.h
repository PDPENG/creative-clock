#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

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
 *	KEY��������
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

/*
	KEY0 	- PD10
	KEY1 	- PD9
	KEY2 	- PD8
	WK_UP 	- PC13
*/
#define KEY0        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10)
#define KEY1        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9)
#define KEY2        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)
#define WK_UP       HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)

#define KEY0_PRES 		1
#define KEY1_PRES		2
#define KEY2_PRES		3
#define WKUP_PRES   	4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
