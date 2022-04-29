#ifndef __LIB_H
#define __LIB_H

#define COLOR_BLAWHI 0x11
#define COLOR_RGB565 0x12
#define COLOR_STYLE COLOR_RGB565

#if(COLOR_STYLE==COLOR_RGB565)
	extern unsigned short Col_Front;//字体前色
	extern unsigned short Col_Back;//字体后色
	extern unsigned short Col_Gronud;//字体背景色
	extern unsigned short Col_Active;//当前秒颜色
#else

#endif

typedef void Fun_Draw_Num(unsigned short x, unsigned short y, unsigned int num, unsigned char len, unsigned char size, unsigned char mode);
extern unsigned char Font_Mode;
extern Fun_Draw_Num *Draw_Num;//绘制数字接口

typedef struct{
	unsigned char Hou;//时分秒
	unsigned char Min;//时分秒
	unsigned char Sec;//时分秒
}_TimeStruct;

extern _TimeStruct Time;

typedef struct{
	unsigned short Weigh;//横向大数字间距
	unsigned char FontSize;//字体大小
	unsigned char FontOffx;//横向数字间距
	unsigned char FontOffy;//纵向数字间距
}_GUIStruct;

extern _GUIStruct GuiStruct;//布局结构体

extern unsigned short * ColPoint,* ColBack;

void Time_Show_Style(unsigned char sx,unsigned char sy);//刷新函数

////////////////////////////////////////////////////////////////分割线哎////////////////////////////////////////////////////////////////
#endif
