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
 *	正点原子 Pandora STM32L475 IoT开发板	实验29
 *	图片显示实验		HAL库版本
 *	技术支持：www.openedv.com
 *	淘宝店铺：http://openedv.taobao.com
 *	关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 *	广州市星翼电子科技有限公司
 *	作者：正点原子 @ALIENTEK
 *	******************************************************************************/

/**
 * @brief	得到path路径下,目标文件的总个数
 *
 * @param   path	路径
 *
 * @return  u16		总有效文件数
 */
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO *tfileinfo;	//临时文件信息	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAM1,sizeof(FILINFO));//申请内存
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,tfileinfo);       		//读取目录下的一个文件  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//错误了/到末尾了,退出	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	}  
	myfree(SRAM1,tfileinfo);//释放内存
	return rval;
}

/*
	注意：本例程图片显示刷新可能会有点慢
*/
char file_name[120];
float x=-25,y=-25;
int main(void)
{
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO *picfileinfo;//文件信息 
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//图片文件offset索引表
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
    HAL_Init();
    SystemClock_Config();		//初始化系统时钟为80M
    delay_init(80); 			//初始化延时函数    80M系统时钟
    uart_init(115200);			//初始化串口，波特率为115200

    LED_Init();					//初始化LED
    KEY_Init();					//初始化按键
    LCD_Init();					//初始化LCD
    W25QXX_Init();				//初始化W25Q256

    my_mem_init(SRAM1);			//初始化内部SRAM1内存池
    my_mem_init(SRAM2);			//初始化内部SRAM2内存池

    exfuns_init();		        //为fatfs相关变量申请内存  
    f_mount(fs[0],"0:",1);      //挂载SD卡 
  	f_mount(fs[1],"1:",1);      //挂载SPI FLASH. 

	RTC_Init();
	//RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);
	POINT_COLOR=RED;
	Display_ALIENTEK_LOGO(0,0);
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,85,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,85,240,101,WHITE);//清除显示	     
		delay_ms(200);				  
	}
 	Show_Str(30,85,200,16,(u8 *)"潘多拉STM32L4 IOT开发板",16);				    	 
	Show_Str(30,105,200,16,(u8 *)"图片显示程序",16);				    	 
	Show_Str(30,125,200,16,(u8 *)"KEY0:NEXT KEY2:PREV",16);				    	 
	Show_Str(30,145,200,16,(u8 *)"KEY_UP:PAUSE",16);				    	 
	Show_Str(30,165,200,16,(u8 *)"正点原子@ALIENTEK",16);				    	 
	Show_Str(30,185,200,16,(u8 *)"2018年10月27日",16);
	piclib_init();										//初始化画图	
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
	
 	while(f_opendir(&picdir,(char *)"1:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,205,240,16,(u8 *)"PICTURE文件夹错误!",16);
		delay_ms(200);				  
		LCD_Fill(30,205,240,221,WHITE);//清除显示
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum((u8 *)"1:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0
 	{
		Show_Str(30,205,240,16,(u8 *)"没有图片文件!",16);
		delay_ms(200);				  
		LCD_Fill(30,205,240,221,WHITE);//清除显示	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAM1,sizeof(FILINFO));	//申请内存
 	pname=mymalloc(SRAM1,_MAX_LFN*2+1);						//为带路径的文件名分配内存
 	picoffsettbl=mymalloc(SRAM1,4*totpicnum);				//申请4*totpicnum个字节的内存,用于存放图片索引
 	while(!picfileinfo||!pname||!picoffsettbl)				//内存分配出错
 	{	    	
		Show_Str(30,205,240,16,(u8 *)"内存分配失败!",16);
		delay_ms(200);				  
		LCD_Fill(30,205,240,221,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"1:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	} 	
	Show_Str(30,205,240,16,(u8 *)"开始显示...",16); 
	delay_ms(1000);   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"1:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		strcpy((char*)pname,"1:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,LCD_Width,LCD_Height,1);//显示图片    
		Show_Str(2,2,LCD_Width,16,pname,16); 				//显示图片名字
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//扫描按键
			if(t>250)key=1;			//模拟一次按下KEY0    
			if((t%20)==0)LED_B_TogglePin;//LED0闪烁,提示程序正在运行.
			if(key==KEY2_PRES)		//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED_R(!pause); 		//暂停的时候LED1亮.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAM1,picfileinfo);			//释放内存						   		    
	myfree(SRAM1,pname);				//释放内存			    
	myfree(SRAM1,picoffsettbl);			//释放内存
}


