#include "sys.h"
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"
#include "string.h"
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
 *	������ʾ ��������
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
 * @brief	���ֿ����ҳ���ģ
 *
 * @param   code 	�ַ�ָ�뿪ʼ
 * @param   mat  	���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes��С
 * @param   size	�����С
 *
 * @return  void
 */
void Get_HzMat(unsigned char *code, unsigned char *mat, u8 size)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    qh = *code;
    ql = *(++code);

    if(qh < 0x81 || ql < 0x40 || ql == 0xff || qh == 0xff) //�� ���ú���
    {
        for(i = 0; i < csize; i++)*mat++ = 0x00; //�������

        return; //��������
    }

    if(ql < 0x7f)ql -= 0x40; //ע��!

    else ql -= 0x41;

    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize;	//�õ��ֿ��е��ֽ�ƫ����

    switch(size)
    {
        case 12:
            W25QXX_Read(mat, foffset + ftinfo.f12addr, csize);
            break;

        case 16:
            W25QXX_Read(mat, foffset + ftinfo.f16addr, csize);
            break;

        case 24:
            W25QXX_Read(mat, foffset + ftinfo.f24addr, csize);
            break;

        case 32:
            W25QXX_Read(mat, foffset + ftinfo.f32addr, csize);
            break;

    }
}

/**
 * @brief	��ʾһ��ָ����С�ĺ���
 *
 * @param   x 		��ʾ������
 * @param   y		��ʾ������
 * @param   font	����GBK��
 * @param   size	�����С
 *
 * @return 	void
 */
void Show_Font(u16 x, u16 y, u8 *font, u8 size)
{
    u16 colortemp;
    u8 sta;
    u8 temp, t, t1;
    u8 dzk[128];
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);			//�õ�����һ���ַ���Ӧ������ռ���ֽ���

    if(size != 12 && size != 16 && size != 24 && size != 32)return;	//��֧�ֵ�size

    Get_HzMat(font, dzk, size);							//�õ���Ӧ��С�ĵ�������

    if((size == 16) || (size == 24) || (size == 32))	//16��24��32������
    {
        sta = 8;

        LCD_Address_Set(x, y, x + size - 1, y + size - 1);

        for(t = 0; t < csize; t++)
        {
            temp = dzk[t];			//�õ���������

            for(t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

    else if(size == 12)	//12������
    {
        LCD_Address_Set(x, y, x + size - 1, y + size - 1);

        for(t = 0; t < csize; t++)
        {
            temp = dzk[t];			//�õ���������

            if(t % 2 == 0)sta = 8;

            else sta = 4;

            for(t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
}
/**
 * @brief	��ָ��λ�ÿ�ʼ��ʾһ���ַ���
 *
 * @param   x 		��ʾ������
 * @param   y		��ʾ������
 * @param   width	��ʾ������
 * @param   height	��ʾ����߶�
 * @param   str  	�ַ���
 * @param   size 	�����С
 *
 * @return 	void
 */
void Show_Str(u16 x, u16 y, u16 width, u16 height, u8*str, u8 size)
{
    u16 x0 = x;
    u16 y0 = y;
    u8 bHz = 0;   //�ַ���������

    while(*str != 0) //����δ����
    {
        if(!bHz)
        {
            if(*str > 0x80)bHz = 1; //����

            else              //�ַ�
            {
                if(x > (x0 + width - size / 2)) //����
                {
                    y += size;
                    x = x0;
                }

                if(y > (y0 + height - size))break; //Խ�緵��

                if(*str == 13) //���з���
                {
                    y += size;
                    x = x0;
                    str++;
                }

                else LCD_ShowChar(x, y, *str, size); //��Ч����д��

                str++;
                x += size / 2; //�ַ�,Ϊȫ�ֵ�һ��
            }
        }

        else //����
        {
            bHz = 0; //�к��ֿ�

            if(x > (x0 + width - size)) //����
            {
                y += size;
                x = x0;
            }

            if(y > (y0 + height - size))break; //Խ�緵��

            Show_Font(x, y, str, size); //��ʾ�������,������ʾ
            str += 2;
            x += size; //��һ������ƫ��
        }
    }
}

/**
 * @brief	��ָ����ȵ��м���ʾ�ַ���������ַ����ȳ�����len,����Show_Str��ʾ
 *
 * @param   x 		��ʾ������
 * @param   y		��ʾ������
 * @param   len		ָ��Ҫ��ʾ�Ŀ��
 *
 * @return 	void
 */
void Show_Str_Mid(u16 x, u16 y, u8*str, u8 size, u8 len)
{
    u16 strlenth = 0;
    strlenth = strlen((const char*)str);
    strlenth *= size / 2;

    if(strlenth > len)Show_Str(x, y, LCD_Width, LCD_Height, str, size);

    else
    {
        strlenth = (len - strlenth) / 2;
        Show_Str(strlenth + x, y, LCD_Width, LCD_Height, str, size);
    }
}
































