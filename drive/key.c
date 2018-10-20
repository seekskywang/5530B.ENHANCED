/******************** (C) COPYRIGHT 2015 AVER********************
 * 文件名  ：KEY.C
 * 作者    ：李振
 * 描述    ：3*2矩阵键盘扫描
 * 内容    ：扫描完成后出键值
 * 硬件连接：一路3*2矩阵键盘  一路编码器独立按键
 * 修改日期：2015-08-18
********************************************************************/
#include "my_register.h"
#include "beep.h"
#include "flash.h"
#include "gpio.h"
#include "key.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "ssd1963.h"
#include  "gui.h"
#include "MainTask.h"

extern WM_HWIN CreateR(void);
extern WM_HWIN CreateWindow2(void);
extern WM_HWIN CreateWindow(void);
extern WM_HWIN CreateG(void);
extern WM_HWIN CreateSET(void);
extern WM_HWIN hWinWind;
extern WM_HWIN hWinR;
extern WM_HWIN load_wind;//璐熻浇鐣岄潰鍙ユ焺
extern WM_HWIN hWinG;
extern WM_HWIN hWinset;
extern WM_HWIN hWincdc;
extern WM_HWIN hWinsysinfo;
extern vu8 para_set2;
extern vu16 s;
extern vu8 cdc_sw;
extern vu8 oct_sw;
extern vu8 timer_sw;
extern vu8 mode_sw;
extern vu8 pow_sw;
extern vu8 load_sw;
extern vu8 admin;
extern vu8 pause_flag;
extern vu16 restart_time;
extern vu8 paused;
extern vu8 finish;
extern vu8 short_finish;
extern float gate_v;
extern vu8 manual;
extern vu8 oc_mode;
vu8 t_MODE;
vu8 pass = 0;
vu8 LOAD_t;
vu8 track = face_r;
vu8 clear_flag1 = 0;
vu8 clear_flag2 = 0;
vu8 clear_flag3 = 0;
vu16 menu_time;
vu16 load_time;
vu8 r_test = 0;
float clear_v = 0;
float clear_pv = 0;
float clear_lv = 0;
vu8 clear_r = 0;
vu8 r_raly=0;

vu8 c_rec;

//=================================================================
extern struct bitDefine
{
    unsigned bit0: 1;
    unsigned bit1: 1;
    unsigned bit2: 1;
    unsigned bit3: 1;
    unsigned bit4: 1;
    unsigned bit5: 1;
    unsigned bit6: 1;
    unsigned bit7: 1;
} flagA, flagB,flagC,flagD,flagE,flagF,flagG;
//====================================================================



#define MAX_VOL  3200		
#define MAX_CUR  5100	
vu8 t_KeyScan;
vu8 NewKey;
vu8 t_wei;
vu8 t_beep;
vu8 t_Str;
vu8 t_lock;
vu8 t_onoff;
vu8 t_doub;
vu8 t_key_step;//閿洏鎵弿璁℃暟鍣?
vu16 Key1,Key2,Key3,Key4,Key5,Key6,Key7,Key8;
vu32 Key9;
vu16 KeyCounter;
vu16 MAX_limit;//自动测试上限值
vu16 MIN_limit;//自动测试下限值
vu16 Test_Time;
vu16 Test_Time1;
vu16 Time_sw;//定时
vu16 Test_Daley;//自动测试判断等待时间，此时间用户可设置
vu16 Test_C_Door;//自动测试门槛电流
vu8 OFFSET_Zore;//零点值
vu32 Key_Scan(void);
void Lift_Move(void);
void Right_Move(void);
void Setvalue_Add(void);
/* 閿�煎畾涔?*/
#define        KEY_1                		 0X4E     //
#define        KEY_2                		 0X56     //
#define        KEY_3                		 0X5E     //
#define        KEY_4                		 0X57      //
#define        KEY_5                		 0X5F      //
#define        KEY_6                		 0X76      //
#define        KEY_7                		 0X67     //
#define        KEY_8                		 0X6F      //
#define        KEY_9                		 0X77     //
#define        KEY_0                		 0X5D      //
#define        KEY_dian              		 0X65     //
#define        KEY_ESC              		 0X6D     //

#define        ENTER                     0X4D    //
#define        PUSH_Lift                 0X45     //
#define        PUSH_Right                0X55      //
#define        PUSH_Up                   0X46      //
#define        PUSH_Down                 0X74      //
#define        PUSH_DISP                 0X47      //
#define        PUSH_SETUP                0X4F     //

#define        KEY_Lock                  0X75     //
#define        KEY_BIAS                  0X6E      //
#define        KEY_Reset                 0X66      //
#define        KEY_TRIG                  0X75      //

#define        KEY_Powe                  0X6C     //
#define        KEY_Face1                 0X64      //
#define        KEY_Face2                 0X5c      //
#define        KEY_Face3                 0X54      //
#define        KEY_Face4                 0X4c      //
#define        KEY_Face5                 0X44      //

/* 瀹氫箟鎸夐敭鍔ㄤ綔 */
#define        KEY_NULL_VALUE             0X00    //无动作
#define        KEY_PRESS                  0X10    //单击
#define        KEY_LONG                   0X20    //长按
#define        KEY_CONTINUE               0X40    //连击
#define        KEY_UP                     0X80    //按键抬起
/*瀹氫箟鎸夐敭澶勭悊姝ラ*/
#define        KEY_INIT_STATE               0      //初态
#define        KEY_WOBBLE_STATE             1      //消抖
#define        KEY_PRESS_STATE              2      //触发单键
#define        KEY_LONG_STATE               3      //长按触发
#define        KEY_CONTINUE_STATE           4      //连续触发
#define        KEY_RELEASE_STATE            5      //恢复初态
/* 闀挎寜鏃堕棿 */
#define         KEY_LONG_PERIOD             50                /* 长按时间1S */
#define        KEY_CONTINUE_PERIOD          10                /* 双击时间500ms */

/************************************************************************************************************************/
vu32 Key_Scan(void)
{
	static vu8 KeyState = KEY_INIT_STATE;
	static vu8 IsKeyRelease = 1;

	static vu8 KeyValueTemp = 0;
	vu8 KeyValue ;
	KeyValue= KEY_NULL_VALUE;
	
	if((NewKey>0x37)&&(IsKeyRelease==1))
	{
		if(KeyState == KEY_INIT_STATE)                    /*无按键按下*/
		{
			KeyState = KEY_WOBBLE_STATE;
			return KEY_NULL_VALUE;
		}
		else if(KeyState == KEY_WOBBLE_STATE)                /* 消抖 */
		{
			KeyState = KEY_PRESS_STATE;
			return KEY_NULL_VALUE;
		}
		else if(KeyState == KEY_PRESS_STATE)                /* 有按键按下，返回按键值 */
		{
			KeyValue = NewKey;
			KeyValueTemp = KeyValue;
			KeyState = KEY_CONTINUE_STATE;
			return KeyValue;
		}
		else if(KeyState == KEY_CONTINUE_STATE)           /*双击按键*/
		{
			KeyCounter ++;
			if(KeyCounter == KEY_CONTINUE_PERIOD)
			{
				 KeyCounter = 0;
				 KeyState = KEY_LONG_STATE;
			}
		}
		else if(KeyState == KEY_LONG_STATE)                        /* 长按按键 */
		{
			KeyCounter ++;
			if(KeyCounter == KEY_LONG_PERIOD)
			{
				KeyCounter = 0;
				Flag_Long=1; //长按触发标志位
			}
		}
	}
	else if(NewKey<=0x37)
	{
		KeyState = KEY_INIT_STATE;        /* 误触发，返回到初始状态 */
		IsKeyRelease = 1;
		Flag_Long=0;
	}
	return KEY_NULL_VALUE;
}
/***************************************************************************************************************************/
void Key_Funtion(void)
{
	vu32 KeyValue = 0;
	static vu8 t_KEYON;
	static vu8 a,b;
/****************************************************鐭寜鍔熻兘************************************************************/
	if(Flag_Key_Scan==1)
	{
		Flag_Key_Scan = 0;
		KeyValue = Key_Scan();
		switch(KeyValue)
		{
			case KEY_1 ://
			{
				switch(page_sw){
                    case face_menu: 
                    {
                        INPUT_POW("1");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_set:
                    {
                         INPUT_NUM("1");
                         KeyCounter = 0;
                         BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                         break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("1");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }   
                    case face_load:
                    {
                        INPUT_LOAD("1");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("1");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(1);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }  
                }
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			
			case KEY_2 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("2");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("2");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("2");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("2");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("2");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(2);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
   
                }
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			
			case KEY_3 ://
			{
                switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("3");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("3");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("3");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("3");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("3");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(3);    
                        }else{
                            pass += 3;
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			
			case KEY_4 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("4");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("4");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("4");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("4");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("4");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(4);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			
			case KEY_5 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("5");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("5");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("5");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("5");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("5");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(5);    
                        }else{
                            pass += 5;
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;	
			
			case KEY_6 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("6");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("6");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("6");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("6");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("6");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    } 
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(6);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			
			case KEY_7 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("7");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("7");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("7");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("7");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("7");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(7);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			
			case KEY_8 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("8");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("8");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("8");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("8");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("8");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(8);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			
			case KEY_9 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("9");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("9");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("9");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("9");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("9");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(9);    
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			
			case KEY_0 ://
			{
				switch(page_sw){
                    case face_menu:  
                    {
                        INPUT_POW("0");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                    }
                    case face_set:
                    {
                        INPUT_NUM("0");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C("0");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        INPUT_LOAD("0");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        INPUT_CDC("0");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            INPUT_INFO(0);    
                        }else{
                            pass += 1;
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }                    
                }
			}
			break;
			
			case KEY_dian ://
			{
				switch(page_sw){
                    case face_menu:
                    {
                        INPUT_POW(".");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_set:
                    {
                        INPUT_NUM(".");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_load:
                    {
                        INPUT_LOAD(".");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            INPUT_C(".");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_cdc:
                    {
                        INPUT_CDC(".");
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }                     
                }
			}
			break;
			
			case KEY_ESC :
			{
              sLCD_WR_REG(0x01);
//                main();
//				timer_sw = 0;
				KeyCounter = 0;
			   BEEP_Tiggr();//瑙﹀彂铚傞福鍣
//                switch(page_sw){
//                    case face_r:
//                    {
//                         if(oc_mode == 0)
//                         {
//                             oc_mode = 1;
//                         }else{
//                             oc_mode = 0;
//                         }
//                    }break;
//                    default:sLCD_WR_REG(0x01);
//                    break;
//                }
			}
			break;
			
			case PUSH_Lift :
			{
				switch(page_sw){
                    case face_set:  //绯荤粺璁剧疆
                    {
                        SET_OP_LEFT();  //璁剧疆閫夐」鍒囨崲
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            SYS_INFO_LEFT();  //璁剧疆閫夐」鍒囨崲
                        } 
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:  //绯荤粺璁剧疆
                    {
                        CDC_OP_LEFT();  //璁剧疆閫夐」鍒囨崲
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			case PUSH_Right :
			{
				switch(page_sw){
                    case face_set:  //绯荤粺璁剧疆
                    {
                        SET_OP_RIGHT();  //璁剧疆閫夐」鍒囨崲
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            SYS_INFO_RIGHT();  //璁剧疆閫夐」鍒囨崲
                        } 
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:  //绯荤粺璁剧疆
                    {
                        CDC_OP_RIGHT();  //璁剧疆閫夐」鍒囨崲
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			case PUSH_Up :
			{
				switch(page_sw){
                    case face_menu:
                    {
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        MENU_OP_UP();
                        break;
                    }                    
                    case face_load:
                    {
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        LOAD_OP_UP();
                        break;
                    }
                    case face_set:
                    {
                        SET_OP_UP();  //璁剧疆閫夐」鍒囨崲
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on && oc_mode ==0)
                        {
                            OC_OP_UP();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }

                    }
                    case face_cdc:
                    {
                        CDC_OP_UP();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            SYS_INFO_UP();  //璁剧疆閫夐」鍒囨崲
                        }                        
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }

                }
			}
			break;
			case PUSH_Down :
			{
				switch(page_sw){
                    case face_menu:
                    {
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        MENU_OP_DOWN();
                        break;
                    }  
                    case face_load:
                    {
                       	KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        LOAD_OP_DOWN();
                        break;
                    }
                    case face_set:  //绯荤粺璁剧疆
                    {
                        SET_OP_DOWN();  //璁剧疆閫夐」鍒囨崲
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on && oc_mode ==0)
                        {
                            OC_OP_DOWN();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_cdc:
                    {
                        CDC_OP_DOWN();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        if(admin == 1)
                        {
                            SYS_INFO_DOWN();  //璁剧疆閫夐」鍒囨崲
                        } 
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
   
                }
			}
			break;
			
			case ENTER ://
			{
				switch(page_sw){
                    case face_menu:
                    {
                        MENU_SET();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }  
                    case face_set:
                    {
                        PARA_SET();  //鍙傛暟璁剧疆
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;                        
                    }
                    case face_r:
                    {
                        if(para_set2 == set_2_on)
                        {
                            OC_SET();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                    case face_load:
                    {
                        LOAD_SET();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        CDC_SET();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_sys_info:
                    {
                        CFM_PASS();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			case PUSH_DISP :
			{
				switch(track)
                {
                    case face_r:
                    {
                        WM_DeleteWindow(hWinR);
                        WM_DeleteWindow(hWinWind);
                        WM_DeleteWindow(hWinG);
                        WM_DeleteWindow(load_wind);
                        WM_DeleteWindow(hWinsysinfo);
                        WM_DeleteWindow(hWincdc);
                        WM_DeleteWindow(hWinset);                       
                        CreateR();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_load:
                    {
                        WM_DeleteWindow(hWinR);
                        WM_DeleteWindow(hWinWind);
                        WM_DeleteWindow(hWinG);
                        WM_DeleteWindow(load_wind);
                        WM_DeleteWindow(hWinsysinfo);
                        WM_DeleteWindow(hWincdc);
                        WM_DeleteWindow(hWinset);
                        CreateWindow2();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_menu:
                    {
                        WM_DeleteWindow(hWinR);
                        WM_DeleteWindow(hWinWind);
                        WM_DeleteWindow(hWinG);
                        WM_DeleteWindow(load_wind);
                        WM_DeleteWindow(hWinsysinfo);
                        WM_DeleteWindow(hWincdc);
                        WM_DeleteWindow(hWinset); 
                        CreateWindow();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_graph:
                    {
                        WM_DeleteWindow(hWinR);
                        WM_DeleteWindow(hWinWind);
                        WM_DeleteWindow(hWinG);
                        WM_DeleteWindow(load_wind);
                        WM_DeleteWindow(hWinsysinfo);
                        WM_DeleteWindow(hWincdc);
                        WM_DeleteWindow(hWinset); 
                        CreateG();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                    case face_cdc:
                    {
                        WM_DeleteWindow(hWinR);
                        WM_DeleteWindow(hWinWind);
                        WM_DeleteWindow(hWinG);
                        WM_DeleteWindow(load_wind);
                        WM_DeleteWindow(hWinsysinfo);
                        WM_DeleteWindow(hWincdc);
                        WM_DeleteWindow(hWinset);
                        CreateCDC();
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        break;
                    }
                }
			}
			break;
			case PUSH_SETUP :
			{
// 				KeyCounter = 0;
// 				BEEP_Tiggr();//
// 				t_MODE++;
// 				if(t_MODE>1)
// 				{
// 					t_MODE=0;
// 				}
// 				if(t_MODE==0)
// 				{
// 					flag_Load_CC=1;//CC模式
// 					GPIO_ResetBits(GPIOC,GPIO_Pin_12);//CC
// 					
// 				}
// 				else if(t_MODE==1)
// 				{
// 					flag_Load_CC=0;//CV模式
// 					GPIO_SetBits(GPIOC,GPIO_Pin_12);//CV
// 				}
                WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset); 
                CreateSET();
                KeyCounter = 0;
                BEEP_Tiggr();//瑙﹀彂铚傞福鍣
			}
			break;
			case KEY_TRIG://电子负载/电源ON/OFF
			{
                switch(page_sw)
                {
                    case face_r:
                    {
//                         if(para_set2 == set_2_on)
//                         {
// //                            static vu8 LOAD_t;
//                             Mode_SW_CONT(0x02);
//                             LOAD_t++;
//                             if(LOAD_t>1)
//                             {
//                                 LOAD_t=0;
//                             }
//                             if(LOAD_t==0)
//                             {
//                                 oct_sw = oct_off;
//                                 set_add_c = 0;
//                                 Flag_Swtich_ON=0;
//                                 GPIO_SetBits(GPIOC,GPIO_Pin_1);//OFF
//                                  
//                             }
//                             else if(LOAD_t==1)
//                             {
//                                 oct_sw = oct_on;
//                                 Flag_Swtich_ON=1;
//                                 GPIO_ResetBits(GPIOC,GPIO_Pin_1);//On
//                             }
//                          }
                         if(r_test == 0 && DISS_Voltage > gate_v)
                         {
                            r_test = 1;
                         }else{
                             r_test = 0;
                         }
                         KeyCounter = 0;
                         BEEP_Tiggr();//触发蜂鸣器
                         break;
                    }
                    case face_load:
                    {
//                         t_onoff++;
//                         if(t_onoff>1)
//                         {
//                             t_onoff=0;
//                         }
                        if(load_sw==load_on)
                        {
                            Flag_Swtich_ON=0;
                            GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                            c_rec = 0;
                            mode_sw = 0;
                            load_sw = load_off;
                        }
                        else if(load_sw==load_off)
                        {
                            Flag_Swtich_ON=1;
                            GPIO_ResetBits(GPIOA,GPIO_Pin_15);//电子负载On
                            if(flag_Load_CC == 0)
                            {
                                c_rec = 1;
                            }
                            mode_sw = mode_load;
                            load_sw = load_on;
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//
                    }break;
                    case face_menu:
                    {
//                         static vu8 POW_t;
//                         POW_t++;
//                         if(POW_t>1)
//                         {
//                             POW_t=0;
//                         }
                        if(pow_sw==pow_on)
                        {
                            GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                            GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                            mode_sw = 0;
                            pow_sw = pow_off;
                        }
                        else if(pow_sw==pow_off)
                        {
                           GPIO_ResetBits(GPIOC,GPIO_Pin_13);//打开电源输出继电器
                            GPIO_SetBits(GPIOC,GPIO_Pin_1);//打开电源输出                           
                            mode_sw = mode_pow;
                            pow_sw = pow_on;
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//
                    }break;
                     case face_cdc:
                    {
                        if(cdc_sw == cdc_off)
                        {
                           SET_Voltage = opv1;
                           SET_Current = opc1;
                           cutoff_flag = 0;
//                           Mode_SW_CONT(0x03);
                           
                           charge_step = 1;
                           GPIO_ResetBits(GPIOC,GPIO_Pin_13);//打开电源输出继电器
                           GPIO_SetBits(GPIOC,GPIO_Pin_1);//打开电源输出
                           mode_sw = mode_pow;
                           cdc_sw = cdc_on;
                        }else{
                           
                           GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                           GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                           cdc_sw = cdc_off;
                           paused = 0;
                           mode_sw = 0;
                        }
                        KeyCounter = 0;
                        BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                       
                    }break;
                }
				
			}
			break;
			case KEY_Reset://电阻高低档位切换
			{
                static vu8 at = 0;
                switch(page_sw)
                {
                    case face_r:
                    {
                        if(at == 0)
                        {
                            GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                            GPIO_ResetBits(GPIOB,GPIO_Pin_13);//R_RALY低档位                           
                            at++;
                            r_raly=0;
                            manual = 1;//手动换档
                        }else if(at == 1){
                            GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                            GPIO_SetBits(GPIOB,GPIO_Pin_13);//R_RALY高档位
                            r_raly=1;
                            at++;
                            manual = 1;//手动换档
                        }else if(at == 2){
                            at = 0;
                            manual =0;
                        }
                    }
                }
                KeyCounter = 0;
				BEEP_Tiggr();//
            }
			break;
			case KEY_Powe :
			{
				
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			case KEY_Face1 :
			{
				WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset);
                GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                CreateR();
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			case KEY_Face2 :
			{
				WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset);
				CreateWindow2();
                t_onoff = 0;
                GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			case KEY_Face3 :
			{
				WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset);
				CreateWindow();
                GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			case KEY_Face4 :
			{
                WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset);
                CreateCDC(); 
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
			case KEY_Face5 :
			{
                WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset);
                CreateG();
				KeyCounter = 0;
				BEEP_Tiggr();//
			}
			break;
            case KEY_BIAS :
			{
				WM_DeleteWindow(hWinR);
                WM_DeleteWindow(hWinWind);
                WM_DeleteWindow(hWinG);
                WM_DeleteWindow(load_wind);
                WM_DeleteWindow(hWinsysinfo);
                WM_DeleteWindow(hWincdc);
                WM_DeleteWindow(hWinset);
                Createsysinfo();
                KeyCounter = 0;
                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
			}
			break;

                    
            
			default:
			break;
		}	
	}
}


void setmode_r(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器  
    GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF  
    pow_sw = pow_off;
    load_sw = load_off;
    mode_sw = mode_r;
}

void IO_OFF(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器  
    GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF  
    pow_sw = pow_off;
    load_sw = load_off;
}
