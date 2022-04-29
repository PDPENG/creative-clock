#include "spi3.h"
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
 *	SPI3��������
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

SPI_HandleTypeDef SPI3_Handler;  //SPI3���


/**
 * @brief	SPI3��ʼ�����룬���ó�����ģʽ
 *
 * @param   void
 *
 * @return  void
 */
void SPI3_Init(void)
{
    SPI3_Handler.Instance=SPI3;                         //SPI3
    SPI3_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
    SPI3_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI3_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI3_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI3_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI3_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI3_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI3_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI3_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
    SPI3_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI3_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI3_Handler);//��ʼ��SPI3
    
    __HAL_SPI_ENABLE(&SPI3_Handler);                    //ʹ��SPI3
}

/**
 * @brief	SPI1��SPI3�ײ�������ʱ��ʹ�ܣ���������
 *
 * @param   hspi	SPI���
 *
 * @return  void
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	if( hspi->Instance == SPI1 )
	{
		GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOA_CLK_ENABLE();       //ʹ��GPIOAʱ��
		__HAL_RCC_SPI1_CLK_ENABLE();        //ʹ��SPI1ʱ��

		//PA5.6.7
		GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
		GPIO_Initure.Pull=GPIO_PULLUP;                  //����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz           
		GPIO_Initure.Alternate=GPIO_AF5_SPI1;           //����ΪSPI1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	}
	
	if( hspi->Instance == SPI3 )
	{
		GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOBʱ��
		__HAL_RCC_SPI3_CLK_ENABLE();        //ʹ��SPI3ʱ��

		//PB3.5
		GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_5;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
		GPIO_Initure.Pull=GPIO_PULLUP;                  //����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz           
		GPIO_Initure.Alternate=GPIO_AF6_SPI3;           //PB3.5����ΪSPI3
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}
}

/**
 * @brief	SPI3 ��дһ���ֽ�
 *
 * @param   TxData	Ҫд����ֽ�
 *
 * @return  u8		��ȡ�����ֽ�
 */
u8 SPI3_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI3_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //�����յ�������		
}

/**
 * @brief	SPI3 д��һ���ֽ�
 *
 * @param   TxData	Ҫд����ֽ�
 * @param   size	д���ֽڴ�С
 *
 * @return  u8		0:д��ɹ�,����:д��ʧ��
 */
u8 SPI3_WriteByte(u8 *TxData,u16 size)
{
	return HAL_SPI_Transmit(&SPI3_Handler,TxData,size,1000);
}















