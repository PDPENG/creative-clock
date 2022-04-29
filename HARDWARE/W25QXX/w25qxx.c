#include "w25qxx.h"
#include "qspi.h"
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
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	W25QXX QSPI��������
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
	4KbytesΪһ��Sector
	16������Ϊ1��Block
	W25Q128����Ϊ16M�ֽ�,����256��Block,4096��Sector
*/
u16 W25QXX_TYPE = W25Q128;	//Ĭ����W25Q128



/**
 * @brief	��ʼ��SPI FLASH��IO��
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Init(void)
{
    QSPI_Init();					//��ʼ��QSPI
    W25QXX_TYPE = W25QXX_ReadID();	//��ȡFLASH ID.
//	printf("ID:%x\r\n",W25QXX_TYPE)
}


/**
 * @brief	��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
 * 			״̬�Ĵ���1��
 * 			BIT7  6   5   4   3   2   1   0
 * 			SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 * 			SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
 * 			TB,BP2,BP1,BP0:FLASH����д��������
 * 			WEL:дʹ������
 * 			BUSY:æ���λ(1,æ;0,����)
 * 			Ĭ��:0x00
 * 			״̬�Ĵ���2��
 * 			BIT7  6   5   4   3   2   1   0
 * 			SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 * 			״̬�Ĵ���3��
 * 			BIT7      6    5    4   3   2   1   0
 * 			HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param   regno	״̬�Ĵ����ţ���:1~3
 *
 * @return  u8		״̬�Ĵ���ֵ
 */
u8 W25QXX_ReadSR(u8 regno)
{
    u8 byte = 0, command = 0;

    switch(regno)
    {
        case 1:
            command = W25X_ReadStatusReg1;  //��״̬�Ĵ���1ָ��
            break;

        case 2:
            command = W25X_ReadStatusReg2;  //��״̬�Ĵ���2ָ��
            break;

        case 3:
            command = W25X_ReadStatusReg3;  //��״̬�Ĵ���3ָ��
            break;

        default:
            command = W25X_ReadStatusReg1;
            break;
    }

    QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(&byte, 1);
    return byte;
}

/**
 * @brief	дW25QXX״̬�Ĵ���
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Write_SR(u8 regno, u8 sr)
{
    u8 command = 0;

    switch(regno)
    {
        case 1:
            command = W25X_WriteStatusReg1;  //д״̬�Ĵ���1ָ��
            break;

        case 2:
            command = W25X_WriteStatusReg2;  //д״̬�Ĵ���2ָ��
            break;

        case 3:
            command = W25X_WriteStatusReg3;  //д״̬�Ĵ���3ָ��
            break;

        default:
            command = W25X_WriteStatusReg1;
            break;
    }

    QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    QSPI_Transmit(&sr, 1);
}


/**
 * @brief	W25QXXдʹ��,��S1�Ĵ�����WEL��λ
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Write_Enable(void)
{
    QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

/**
 * @brief	W25QXXд��ֹ,��WEL����
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Write_Disable(void)
{
    QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

/**
 * @brief	��ȡW25QXXоƬID
 *
 * @param   void
 *
 * @return  u8	����ֵ����
 * 				0XEF13,��ʾоƬ�ͺ�ΪW25Q80
 * 				0XEF14,��ʾоƬ�ͺ�ΪW25Q16
 * 				0XEF15,��ʾоƬ�ͺ�ΪW25Q32
 * 				0XEF16,��ʾоƬ�ͺ�ΪW25Q64
 * 				0XEF17,��ʾоƬ�ͺ�ΪW25Q128
 * 				0XEF18,��ʾоƬ�ͺ�ΪW25Q256
 */
u16 W25QXX_ReadID(void)
{
    u8 temp[2];
    u16 deviceid;
    QSPI_Send_CMD(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];
    return deviceid;
}


/**
 * @brief	��ȡSPI FLASH����
 *
 * @param   pBuffer			���ݴ洢��
 * @param   ReadAddr		��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param   NumByteToRead	Ҫ��ȡ���ֽ���(���65535)
 *
 * @return  void
 */
void W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    QSPI_Send_CMD(W25X_FastReadData, ReadAddr, 8, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(pBuffer, NumByteToRead);
}


/**
 * @brief	��ָ����ַ��ʼд�����256�ֽڵ�����
 *
 * @param   pBuffer			���ݴ洢��
 * @param   WriteAddr		��ʼд��ĵ�ַ(���32bit)
 * @param   NumByteToWrite	Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
 *
 * @return  void
 */
void W25QXX_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    W25QXX_Write_Enable();					//дʹ��
    QSPI_Send_CMD(W25X_PageProgram, WriteAddr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Transmit(pBuffer, NumByteToWrite);
    W25QXX_Wait_Busy();					   //�ȴ�д�����
}

/**
 * @brief	�޼���дSPI FLASH
 * 			����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
 * 			�����Զ���ҳ����
 * 			��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
 *
 * @param   pBuffer			���ݴ洢��
 * @param   WriteAddr		��ʼд��ĵ�ַ(���32bit)
 * @param   NumByteToWrite	Ҫд����ֽ���(���65535)
 *
 * @return  void
 */
void W25QXX_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���

    if(NumByteToWrite <= pageremain)pageremain = NumByteToWrite; //������256���ֽ�

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);

        if(NumByteToWrite == pageremain)break; //д�������

        else //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;			 //��ȥ�Ѿ�д���˵��ֽ���

            if(NumByteToWrite > 256)pageremain = 256; //һ�ο���д��256���ֽ�

            else pageremain = NumByteToWrite; 	 //����256���ֽ���
        }
    }
}


/**
 * @brief	дSPI FLASH
 * 			��ָ����ַ��ʼд��ָ�����ȵ�����
 * 			�ú�������������!
 *
 * @param   pBuffer			���ݴ洢��
 * @param   WriteAddr		��ʼд��ĵ�ַ(���32bit)
 * @param   NumByteToWrite	Ҫд����ֽ���(���65535)
 *
 * @return  void
 */
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr / 4096; //������ַ
    secoff = WriteAddr % 4096; //�������ڵ�ƫ��
    secremain = 4096 - secoff; //����ʣ��ռ��С

    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
    if(NumByteToWrite <= secremain)secremain = NumByteToWrite; //������4096���ֽ�

    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //������������������

        for(i = 0; i < secremain; i++) //У������
        {
            if(W25QXX_BUF[secoff + i] != 0XFF)break; //��Ҫ����
        }

        if(i < secremain) //��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);//�����������

            for(i = 0; i < secremain; i++)	 //����
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }

            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //д����������

        }

        else W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //д�Ѿ������˵�,ֱ��д������ʣ������.

        if(NumByteToWrite == secremain)break; //д�������

        else//д��δ����
        {
            secpos++;//������ַ��1
            secoff = 0; //ƫ��λ��Ϊ0

            pBuffer += secremain; //ָ��ƫ��
            WriteAddr += secremain; //д��ַƫ��
            NumByteToWrite -= secremain;				//�ֽ����ݼ�

            if(NumByteToWrite > 4096)secremain = 4096;	//��һ����������д����

            else secremain = NumByteToWrite;			//��һ����������д����
        }
    };
}

/**
 * @brief	��������оƬ,�ȴ�ʱ�䳬��...
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();					//SET WEL
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_ChipErase, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
    W25QXX_Wait_Busy();						//�ȴ�оƬ��������
}


/**
 * @brief	����һ������������һ������������ʱ��:150ms
 *
 * @param   Dst_Addr	������ַ ����ʵ����������
 *
 * @return  void
 */
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
	//printf("fe:%x\r\n",Dst_Addr);			//����flash�������,������
	Dst_Addr *= 4096;
	W25QXX_Write_Enable();                  //SET WEL
	W25QXX_Wait_Busy();
	QSPI_Send_CMD(W25X_SectorErase, Dst_Addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);
	W25QXX_Wait_Busy();   				    //�ȴ��������
}

/**
 * @brief	�ȴ�����
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR(1) & 0x01) == 0x01); // �ȴ�BUSYλ���
}







