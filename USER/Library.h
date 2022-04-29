#ifndef __LIB_H
#define __LIB_H

#define COLOR_BLAWHI 0x11
#define COLOR_RGB565 0x12
#define COLOR_STYLE COLOR_RGB565

#if(COLOR_STYLE==COLOR_RGB565)
	extern unsigned short Col_Front;//����ǰɫ
	extern unsigned short Col_Back;//�����ɫ
	extern unsigned short Col_Gronud;//���屳��ɫ
	extern unsigned short Col_Active;//��ǰ����ɫ
#else

#endif

typedef void Fun_Draw_Num(unsigned short x, unsigned short y, unsigned int num, unsigned char len, unsigned char size, unsigned char mode);
extern unsigned char Font_Mode;
extern Fun_Draw_Num *Draw_Num;//�������ֽӿ�

typedef struct{
	unsigned char Hou;//ʱ����
	unsigned char Min;//ʱ����
	unsigned char Sec;//ʱ����
}_TimeStruct;

extern _TimeStruct Time;

typedef struct{
	unsigned short Weigh;//��������ּ��
	unsigned char FontSize;//�����С
	unsigned char FontOffx;//�������ּ��
	unsigned char FontOffy;//�������ּ��
}_GUIStruct;

extern _GUIStruct GuiStruct;//���ֽṹ��

extern unsigned short * ColPoint,* ColBack;

void Time_Show_Style(unsigned char sx,unsigned char sy);//ˢ�º���

////////////////////////////////////////////////////////////////�ָ��߰�////////////////////////////////////////////////////////////////
#endif
