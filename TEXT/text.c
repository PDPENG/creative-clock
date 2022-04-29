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
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	汉字显示 驱动代码
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


/**
 * @brief	从字库中找出字模
 *
 * @param   code 	字符指针开始
 * @param   mat  	数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小
 * @param   size	字体大小
 *
 * @return  void
 */
void Get_HzMat(unsigned char *code, unsigned char *mat, u8 size)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); //得到字体一个字符对应点阵集所占的字节数
    qh = *code;
    ql = *(++code);

    if(qh < 0x81 || ql < 0x40 || ql == 0xff || qh == 0xff) //非 常用汉字
    {
        for(i = 0; i < csize; i++)*mat++ = 0x00; //填充满格

        return; //结束访问
    }

    if(ql < 0x7f)ql -= 0x40; //注意!

    else ql -= 0x41;

    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize;	//得到字库中的字节偏移量

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
 * @brief	显示一个指定大小的汉字
 *
 * @param   x 		显示横坐标
 * @param   y		显示纵坐标
 * @param   font	汉字GBK码
 * @param   size	字体大小
 *
 * @return 	void
 */
void Show_Font(u16 x, u16 y, u8 *font, u8 size)
{
    u16 colortemp;
    u8 sta;
    u8 temp, t, t1;
    u8 dzk[128];
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);			//得到字体一个字符对应点阵集所占的字节数

    if(size != 12 && size != 16 && size != 24 && size != 32)return;	//不支持的size

    Get_HzMat(font, dzk, size);							//得到相应大小的点阵数据

    if((size == 16) || (size == 24) || (size == 32))	//16、24、32号字体
    {
        sta = 8;

        LCD_Address_Set(x, y, x + size - 1, y + size - 1);

        for(t = 0; t < csize; t++)
        {
            temp = dzk[t];			//得到点阵数据

            for(t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

    else if(size == 12)	//12号字体
    {
        LCD_Address_Set(x, y, x + size - 1, y + size - 1);

        for(t = 0; t < csize; t++)
        {
            temp = dzk[t];			//得到点阵数据

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
 * @brief	在指定位置开始显示一个字符串
 *
 * @param   x 		显示横坐标
 * @param   y		显示纵坐标
 * @param   width	显示区域宽度
 * @param   height	显示区域高度
 * @param   str  	字符串
 * @param   size 	字体大小
 *
 * @return 	void
 */
void Show_Str(u16 x, u16 y, u16 width, u16 height, u8*str, u8 size)
{
    u16 x0 = x;
    u16 y0 = y;
    u8 bHz = 0;   //字符或者中文

    while(*str != 0) //数据未结束
    {
        if(!bHz)
        {
            if(*str > 0x80)bHz = 1; //中文

            else              //字符
            {
                if(x > (x0 + width - size / 2)) //换行
                {
                    y += size;
                    x = x0;
                }

                if(y > (y0 + height - size))break; //越界返回

                if(*str == 13) //换行符号
                {
                    y += size;
                    x = x0;
                    str++;
                }

                else LCD_ShowChar(x, y, *str, size); //有效部分写入

                str++;
                x += size / 2; //字符,为全字的一半
            }
        }

        else //中文
        {
            bHz = 0; //有汉字库

            if(x > (x0 + width - size)) //换行
            {
                y += size;
                x = x0;
            }

            if(y > (y0 + height - size))break; //越界返回

            Show_Font(x, y, str, size); //显示这个汉字,空心显示
            str += 2;
            x += size; //下一个汉字偏移
        }
    }
}

/**
 * @brief	在指定宽度的中间显示字符串，如果字符长度超过了len,则用Show_Str显示
 *
 * @param   x 		显示横坐标
 * @param   y		显示纵坐标
 * @param   len		指定要显示的宽度
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
































