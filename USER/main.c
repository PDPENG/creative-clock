#include "sys.h"
#include "usart.h"
#include "usmart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "w25qxx.h"
#include "sd_card.h"
#include "rtc.h"
#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"
#include "Library.h"
/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	����ԭ�� Pandora STM32L475 IoT������	ʵ��29
 *	ͼƬ��ʾʵ��		HAL��汾
 *	����֧�֣�www.openedv.com
 *	�Ա����̣�http://openedv.taobao.com
 *	��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 *	������������ӿƼ����޹�˾
 *	���ߣ�����ԭ�� @ALIENTEK
 *	******************************************************************************/

/**
 * @brief	�õ�path·����,Ŀ���ļ����ܸ���
 *
 * @param   path	·��
 *
 * @return  u16		����Ч�ļ���
 */
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO *tfileinfo;	//��ʱ�ļ���Ϣ	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAM1,sizeof(FILINFO));//�����ڴ�
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼ 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//������/��ĩβ��,�˳�	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	}  
	myfree(SRAM1,tfileinfo);//�ͷ��ڴ�
	return rval;
}

/*
	ע�⣺������ͼƬ��ʾˢ�¿��ܻ��е���
*/
char file_name[120];
float x=-25,y=-25;
int main(void)
{
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO *picfileinfo;//�ļ���Ϣ 
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//ͼƬ�ļ�offset������
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
    HAL_Init();
    SystemClock_Config();		//��ʼ��ϵͳʱ��Ϊ80M
    delay_init(80); 			//��ʼ����ʱ����    80Mϵͳʱ��
    uart_init(115200);			//��ʼ�����ڣ�������Ϊ115200

    LED_Init();					//��ʼ��LED
    KEY_Init();					//��ʼ������
    LCD_Init();					//��ʼ��LCD
    W25QXX_Init();				//��ʼ��W25Q256

    my_mem_init(SRAM1);			//��ʼ���ڲ�SRAM1�ڴ��
    my_mem_init(SRAM2);			//��ʼ���ڲ�SRAM2�ڴ��

    exfuns_init();		        //Ϊfatfs��ر��������ڴ�  
    f_mount(fs[0],"0:",1);      //����SD�� 
  	f_mount(fs[1],"1:",1);      //����SPI FLASH. 

	RTC_Init();
	//RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);
	POINT_COLOR=RED;
	Display_ALIENTEK_LOGO(0,0);
	while(font_init()) 			//����ֿ�
	{	    
		LCD_ShowString(30,85,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,85,240,101,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
 	Show_Str(30,85,200,16,(u8 *)"�˶���STM32L4 IOT������",16);				    	 
	Show_Str(30,105,200,16,(u8 *)"ͼƬ��ʾ����",16);				    	 
	Show_Str(30,125,200,16,(u8 *)"KEY0:NEXT KEY2:PREV",16);				    	 
	Show_Str(30,145,200,16,(u8 *)"KEY_UP:PAUSE",16);				    	 
	Show_Str(30,165,200,16,(u8 *)"����ԭ��@ALIENTEK",16);				    	 
	Show_Str(30,185,200,16,(u8 *)"2018��10��27��",16);
	piclib_init();										//��ʼ����ͼ	
	delay_ms(1000);
	LCD_Clear(BLACK);
	BACK_COLOR = BLACK;
	
	Draw_Num = (Fun_Draw_Num *)LCD_ShowxNum;
	Col_Active = RED;
	Col_Back = 5 << 11 | 10 << 5 | 5;
	Col_Gronud = BLACK;
	Col_Front = WHITE;
	
	Font_Mode = 0x01;
	
	ColBack = &BACK_COLOR;
	ColPoint = &POINT_COLOR;
	
	GuiStruct.FontOffx = 2;
	GuiStruct.FontOffy = 2;
	GuiStruct.FontSize = 16;
	GuiStruct.Weigh = 60;

	while(1)
	{
		HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
		Time.Hou = RTC_TimeStruct.Hours;
		Time.Min = RTC_TimeStruct.Minutes;
		Time.Sec = RTC_TimeStruct.Seconds;
		Time_Show_Style(2,60);
		delay_ms(50);
		key=KEY_Scan(1);
		if(key == WKUP_PRES)
		{
			break;
		}
	}
	
 	while(f_opendir(&picdir,(char *)"1:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,205,240,16,(u8 *)"PICTURE�ļ��д���!",16);
		delay_ms(200);				  
		LCD_Fill(30,205,240,221,WHITE);//�����ʾ
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum((u8 *)"1:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0
 	{
		Show_Str(30,205,240,16,(u8 *)"û��ͼƬ�ļ�!",16);
		delay_ms(200);				  
		LCD_Fill(30,205,240,221,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAM1,sizeof(FILINFO));	//�����ڴ�
 	pname=mymalloc(SRAM1,_MAX_LFN*2+1);						//Ϊ��·�����ļ��������ڴ�
 	picoffsettbl=mymalloc(SRAM1,4*totpicnum);				//����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(!picfileinfo||!pname||!picoffsettbl)				//�ڴ�������
 	{	    	
		Show_Str(30,205,240,16,(u8 *)"�ڴ����ʧ��!",16);
		delay_ms(200);				  
		LCD_Fill(30,205,240,221,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"1:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.dptr;								//��¼��ǰdptrƫ��
	        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picoffsettbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	} 	
	Show_Str(30,205,240,16,(u8 *)"��ʼ��ʾ...",16); 
	delay_ms(1000);   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"1:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�
		strcpy((char*)pname,"1:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)picfileinfo->fname);//���ļ������ں���
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,LCD_Width,LCD_Height,1);//��ʾͼƬ    
		Show_Str(2,2,LCD_Width,16,pname,16); 				//��ʾͼƬ����
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//ɨ�谴��
			if(t>250)key=1;			//ģ��һ�ΰ���KEY0    
			if((t%20)==0)LED_B_TogglePin;//LED0��˸,��ʾ������������.
			if(key==KEY2_PRES)		//��һ��
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//��һ��
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED_R(!pause); 		//��ͣ��ʱ��LED1��.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAM1,picfileinfo);			//�ͷ��ڴ�						   		    
	myfree(SRAM1,pname);				//�ͷ��ڴ�			    
	myfree(SRAM1,picoffsettbl);			//�ͷ��ڴ�
}


