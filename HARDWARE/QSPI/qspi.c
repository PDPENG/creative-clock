#include "qspi.h"
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
 *	QSPI��������
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

QSPI_HandleTypeDef QSPI_Handler;    //QSPI���

/**
 * @brief	QSPI��ʼ�����룬���ó�����ģʽ
 *
 * @param   void
 *
 * @return  u8	0:��ʼ���ɹ���1:��ʼ��ʧ��
 */
u8 QSPI_Init(void)
{
    QSPI_Handler.Instance = QUADSPI;                        //QSPI
    QSPI_Handler.Init.ClockPrescaler = 0;                   //QPSI��Ƶ�ȣ�W25Q128���Ƶ��Ϊ104M��
    //���Դ˴�Ӧ��Ϊ0��QSPIƵ�ʾ�Ϊ80/(0+1)=40MHZ
    QSPI_Handler.Init.FifoThreshold = 4;                    //FIFO��ֵΪ4���ֽ�
    QSPI_Handler.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE; //������λ�������(DDRģʽ��,��������Ϊ0)
    QSPI_Handler.Init.FlashSize = POSITION_VAL(0x1000000) - 1; //SPI FLASH��С��W25Q128��СΪ16M�ֽ�
    QSPI_Handler.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE; //Ƭѡ�ߵ�ƽʱ��Ϊ4��ʱ��(12.5*4=50ns),���ֲ������tSHSL����
    QSPI_Handler.Init.ClockMode = QSPI_CLOCK_MODE_0;        //ģʽ0
    if(HAL_QSPI_Init(&QSPI_Handler) == HAL_OK) return 0;    //QSPI��ʼ���ɹ�
    else return 1;
}
/**
 * @brief	QSPI�ײ�����,�������ã�ʱ��ʹ��
 *
 * @param   hqspi	QSPI���
 *
 * @return  void
 */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_QSPI_CLK_ENABLE();        //ʹ��QSPIʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();       //ʹ��GPIOEʱ��

    /**QUADSPI GPIO Configuration
    PE10     ------> QUADSPI_CLK
    PE11     ------> QUADSPI_NCS
    PE12     ------> QUADSPI_BK1_IO0
    PE13     ------> QUADSPI_BK1_IO1
    PE14     ------> QUADSPI_BK1_IO2
    PE15     ------> QUADSPI_BK1_IO3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13
                          | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
/**
 * @brief	QSPI��������
 *
 * @param   instruction		Ҫ���͵�ָ��
 * @param   address			���͵���Ŀ�ĵ�ַ
 * @param   dummyCycles		��ָ��������
 * @param   instructionMode	ָ��ģʽ;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE
 * @param   addressMode		��ַģʽ; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
 * @param   addressSize		��ַ����;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
 * @param   dataMode		����ģʽ; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE
 *
 * @return  void
 */
void QSPI_Send_CMD(u32 instruction, u32 address, u32 dummyCycles, u32 instructionMode, u32 addressMode, u32 addressSize, u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;

    Cmdhandler.Instruction = instruction;                 	//ָ��
    Cmdhandler.Address = address;                            	//��ַ
    Cmdhandler.DummyCycles = dummyCycles;                   //���ÿ�ָ��������
    Cmdhandler.InstructionMode = instructionMode;				//ָ��ģʽ
    Cmdhandler.AddressMode = addressMode;   					//��ַģʽ
    Cmdhandler.AddressSize = addressSize;   					//��ַ����
    Cmdhandler.DataMode = dataMode;             				//����ģʽ
    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;       	//ÿ�ζ�����ָ��
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; //�޽����ֽ�
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;           	//�ر�DDRģʽ
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    HAL_QSPI_Command(&QSPI_Handler, &Cmdhandler, 5000);
}
/**
 * @brief	QSPI����ָ�����ȵ�����
 *
 * @param   buf				�������ݻ������׵�ַ
 * @param   datalen			Ҫ��������ݳ���
 *
 * @return  u8				0:����;����:�������
 */
u8 QSPI_Receive(u8* buf, u32 datalen)
{
    QSPI_Handler.Instance->DLR = datalen - 1;                       //�������ݳ���
    if(HAL_QSPI_Receive(&QSPI_Handler, buf, 5000) == HAL_OK) return 0; //��������
    else return 1;
}
/**
 * @brief	QSPI����ָ�����ȵ�����
 *
 * @param   buf				�������ݻ������׵�ַ
 * @param   datalen			Ҫ��������ݳ���
 *
 * @return  u8				0:����;����:�������
 */
u8 QSPI_Transmit(u8* buf, u32 datalen)
{
    QSPI_Handler.Instance->DLR = datalen - 1;                        //�������ݳ���
    if(HAL_QSPI_Transmit(&QSPI_Handler, buf, 5000) == HAL_OK) return 0; //��������
    else return 1;
}
